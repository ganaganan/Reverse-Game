#include "Enemy.h"

#include <framework.h>

#include "Scene.h"
#include "FilePath.h"
#include "Light.h"
#include "Camera.h"

int Enemy::ENEMY1_WAIT_COUNT;
int Enemy::ENEMY2_WAIT_COUNT;
int Enemy::ENEMY3_WAIT_COUNT;
int Enemy::ERASE_COUNT;

/*------------------------------------*/
//	各ポイントの位置情報
/*------------------------------------*/
DirectX::XMFLOAT3 pointPosition[] = {
	{  38.0f, 10.0f, 80.0f },	// R_Point1
	{  38.0f, 10.0f, 100.0f },	// R_Point2
	{  38.0f, 10.0f, 120.0f },	// R_Point3
	{  38.0f, 10.0f, 140.0f },	// R_Point4
	{  38.0f, 10.0f, 160.0f },	// R_Point5

	{ -38.0f, 10.0f, 80.0f },	// L_Point1
	{ -38.0f, 10.0f, 100.0f },	// L_Point2
	{ -38.0f, 10.0f, 120.0f },	// L_Point3
	{ -38.0f, 10.0f, 140.0f },	// L_Point4
	{ -38.0f, 10.0f, 160.0f },	// L_Point5
};

/*------------------------------------*/
//	Managerの初期化
/*------------------------------------*/
void EnemyManager::Init()
{
	totemPoleModel = std::make_unique<SkinnedMesh>(FRAMEWORK->GetDevice(), GetModelPath(ModelAttribute::TotemPole));
	ghostModel = std::make_unique<SkinnedMesh>(FRAMEWORK->GetDevice(), GetModelPath(ModelAttribute::Ghost));
	darumaModel = std::make_unique<SkinnedMesh>(FRAMEWORK->GetDevice(), GetModelPath(ModelAttribute::Daruma));

	for (auto& it : enemy)
	{
		it.Init();
	}

	Enemy::ENEMY1_WAIT_COUNT = 250;
	Enemy::ENEMY2_WAIT_COUNT = 200;
	Enemy::ENEMY3_WAIT_COUNT = 300;
	Enemy::ERASE_COUNT = 72;

	// Set Sound
	sound[SoundType::MoveDaruma] = new Audio("Data/Sound/だるま.wav");
	sound[SoundType::MoveGhost] = new Audio("Data/Sound/幽霊飛んでる音.wav");
	sound[SoundType::MovePole] = new Audio("Data/Sound/重い金属引きずる音.wav");
	for (auto& it : soundVol)it = 1;
}

/*------------------------------------*/
//	Managerの更新
/*------------------------------------*/
void EnemyManager::Update()
{
	for (auto& it : enemy)
	{
		it.Update();
	}
#ifdef USE_IMGUI
	ImGui::Begin("enemy pos");
	if (ImGui::TreeNode("Move Speed"))
	{
		ImGui::InputInt(" Daruma ",		&Enemy::ENEMY1_WAIT_COUNT);
		ImGui::InputInt(" Ghost ",		&Enemy::ENEMY2_WAIT_COUNT);
		ImGui::InputInt(" Totempole ",	&Enemy::ENEMY3_WAIT_COUNT);

		ImGui::TreePop();
	}
	ImGui::NewLine();

	ImGui::InputInt(" ERASE_COUNT ", &Enemy::ERASE_COUNT);

	if (ImGui::TreeNode("Sound Volume"))
	{
		ImGui::SliderFloat(" Daruma Move Vol ", &soundVol[SoundType::MoveDaruma], 0.0f, 1.0f);
		ImGui::SliderFloat(" Ghost Move Vol ", &soundVol[SoundType::MoveGhost], 0.0f, 1.0f);
		ImGui::SliderFloat(" Pole Move Vol ", &soundVol[SoundType::MoveGhost], 0.0f, 1.0f);
		ImGui::TreePop();
	}

	ImGui::End();
#endif
}

/*------------------------------------*/
//	Managerの描画
/*------------------------------------*/
void EnemyManager::Render(DirectX::XMMATRIX& _V, DirectX::XMMATRIX& _P)
{
	for (auto& it : enemy)
	{
		it.Render(_V, _P);
	}
}

/*------------------------------------*/
//	Managerの終了処理
/*------------------------------------*/
void EnemyManager::Uninit()
{
	for (auto& it : enemy)
	{
		it.Uninit();
	}

	for (auto& it : sound)
	{
		delete it;
		it = nullptr;
	}

}

/*------------------------------------*/
//	敵の生成
/*------------------------------------*/
void EnemyManager::GenerateEnemy(
	Enemy::EnemyType _type, Enemy::Point _generatePoint)
{
	for (auto& it : enemy)
	{
		if (it.GetEnable())continue;

		it.Generate(
			_type, _generatePoint
		);
		break;
	}
}


/*------------------------------------*/
//	Enemyの初期化
/*------------------------------------*/
void Enemy::Init()
{
	pos = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
	scale = DirectX::XMFLOAT3(0.1f, 0.1f, 0.1f);
	type = EnemyType::TotemPole;
	nowPoint = Point::R_Point1;
	moveTime = 0;
	eraseCount = 0;
	isEnable = false;
	isPreparationErase = false;
	isHitLight = false;

}

/*------------------------------------*/
//	Enemyの更新
/*------------------------------------*/
void Enemy::Update()
{
	if (!isEnable)return;

	Move();

	JudgeIsHitLight();
}

/*------------------------------------*/
//	Enemyの描画
/*------------------------------------*/
void Enemy::Render(DirectX::XMMATRIX& _V, DirectX::XMMATRIX& _P)
{
	if (!isEnable)return;


	using namespace DirectX;
	auto Matrix = [](const XMFLOAT4X4 & matrix)
	{
		return XMLoadFloat4x4(&matrix);
	};
	auto Float4x4 = [](const XMMATRIX & M)
	{
		XMFLOAT4X4 matrix{};
		XMStoreFloat4x4(&matrix, M);
		return matrix;
	};

	XMMATRIX S, R, T, W, WVP;
	S = XMMatrixScaling(scale.x, scale.y, scale.z);
	R = XMMatrixRotationRollPitchYaw(0.0f,XMConvertToRadians(180.0f),0.0f);//XMMatrixIdentity();
	T = XMMatrixTranslation(pos.x, pos.y, pos.z);
	W = S * R * T;

	WVP = W * _V * _P;

	XMFLOAT4 color{ 1.0f,1.0f,1.0f,1.0f };
	XMFLOAT4 light(1.0f, -1.0f, 1.0f, 0.0f);

	switch (type)
	{
	case Enemy::EnemyType::TotemPole:
		EnemyManager::Get().totemPoleModel->Render(
			FRAMEWORK->GetDeviceContext(),
			Float4x4(WVP),
			Float4x4(W),
			light,
			color
		);
		break;
	case Enemy::EnemyType::Ghost:
		EnemyManager::Get().ghostModel->Render(
			FRAMEWORK->GetDeviceContext(),
			Float4x4(WVP),
			Float4x4(W),
			light,
			color
		);
		break;
	case Enemy::EnemyType::Daruma:
		EnemyManager::Get().darumaModel->Render(
			FRAMEWORK->GetDeviceContext(),
			Float4x4(WVP),
			Float4x4(W),
			light,
			color
		);
		break;
	default:
		break;
	}
}

/*------------------------------------*/
//	Enemyの終了処理
/*------------------------------------*/
void Enemy::Uninit()
{

}

/*------------------------------------*/
//	Enemyの移動関数
/*------------------------------------*/
void Enemy::Move()
{
	moveTime++;

	switch (type)
	{
	case Enemy::EnemyType::TotemPole:
		MoveEnemy3();
		break;
	case Enemy::EnemyType::Ghost:
		MoveEnemy2();
		break;
	case Enemy::EnemyType::Daruma:
		MoveEnemy1();
		break;
	default:
		break;
	}
}

void Enemy::MoveEnemy1()
{
	if (moveTime >= ENEMY1_WAIT_COUNT)
	{
		if (nowPoint >= Point::R_Point1 && Point::R_Point5 >= nowPoint)
		{
			MoveRightPoint();
		}
		else
		{
			MoveLeftPoint();

		}
		pos = pointPosition[nowPoint];
		pos.y = 0.0f;
		moveTime = 0;
		EnemyManager::Get().sound[EnemyManager::SoundType::MoveDaruma]->SetPosition(pos);
		EnemyManager::Get().sound[EnemyManager::SoundType::MoveDaruma]->Play(false);
	}
}

void Enemy::MoveEnemy2()
{
	if (moveTime >= ENEMY2_WAIT_COUNT)
	{
		if (nowPoint >= Point::R_Point1 && Point::R_Point5 >= nowPoint)
		{
			MoveRightPoint();
		}
		else
		{
			MoveLeftPoint();

		}
		pos = pointPosition[nowPoint];
		moveTime = 0;
		EnemyManager::Get().sound[EnemyManager::SoundType::MoveGhost]->SetPosition(pos);
		EnemyManager::Get().sound[EnemyManager::SoundType::MoveGhost]->Play(false);
	}
}

void Enemy::MoveEnemy3()
{
	if (moveTime >= ENEMY3_WAIT_COUNT)
	{
		if (rand() % 3 == 0)
		{
			// まっすぐ
			if (nowPoint >= Point::R_Point1 && Point::R_Point5 >= nowPoint)
			{
				MoveRightPoint();
			}
			else
			{
				MoveLeftPoint();

			}
		}
		else
		{
			// 左右
			MoveCross();
		}
		pos = pointPosition[nowPoint];
		pos.y = 0.0f;
		moveTime = 0;
		EnemyManager::Get().sound[EnemyManager::SoundType::MovePole]->SetPosition(pos);
		EnemyManager::Get().sound[EnemyManager::SoundType::MovePole]->Play(false);

	}
}


/*------------------------------------*/
//	Enemyの生成
/*------------------------------------*/
void Enemy::Generate(
	EnemyType _type, Point _generatePoint)
{
	pos = pointPosition[_generatePoint];
	if (_type == EnemyType::Daruma) pos.y = 0.0f;
	type = _type;
	if (type == EnemyType::Ghost)	scale = DirectX::XMFLOAT3(0.1f, 0.1f, 0.1f);
	else							scale = DirectX::XMFLOAT3(0.1f, 0.1f, 0.1f);
	nowPoint = _generatePoint;
	isEnable = true;
	isPreparationErase = false;
	isHitLight = false;
}

/*------------------------------------*/
//	ライトに当たっているかを返す
/*------------------------------------*/
void Enemy::JudgeIsHitLight()
{
	if (!isEnable) return;

	switch (nowPoint)
	{
	case Point::R_Point1:
	case Point::R_Point2:
		if (Light::JudgeLightEnable(1))
		{
			// ライトにあたっている
			if (!isPreparationErase)
			{
				isPreparationErase = true;
				eraseCount = 0;

			}
			EraseThisInstance();
		}
		break;
	case Point::L_Point1:
	case Point::L_Point2:
		if (Light::JudgeLightEnable(0))
		{
			// ライトにあたっている
			if (!isPreparationErase)
			{
				isPreparationErase = true;
				eraseCount = 0;
			}
			EraseThisInstance();
		}
		break;

	default:
		break;
	}
}

/*------------------------------------*/
//	右側の移動関数
/*------------------------------------*/
void Enemy::MoveRightPoint()
{
	if (Camera::Get().GetCanPushSwitch() && Camera::Get().GetMoveState() == Camera::MoveState::Shift_Right)return;

	switch (nowPoint)
	{
	case Point::R_Point5:
		nowPoint = Point::R_Point4;
		break;
	case Point::R_Point4:
		nowPoint = Point::R_Point3;
		break;
	case Point::R_Point3:
		nowPoint = Point::R_Point2;
		break;
	case Point::R_Point2:
		nowPoint = Point::R_Point1;
		break;
	case Point::R_Point1:
//		nowPoint = Point::R_Point4;
		switch (type)
		{
		case Enemy::EnemyType::TotemPole:
			BaseScene::lastEnemyType = 2;
			break;
		case Enemy::EnemyType::Ghost:
			BaseScene::lastEnemyType = 1;
			break;
		case Enemy::EnemyType::Daruma:
			BaseScene::lastEnemyType = 0;
			break;
		default:
			break;
		}
		SceneManager::Get().SetScene(SceneManager::GAME_OVER);
		
		break;
	}
}

/*------------------------------------*/
//	左側の移動関数
/*------------------------------------*/
void Enemy::MoveLeftPoint()
{
	if (Camera::Get().GetCanPushSwitch() && Camera::Get().GetMoveState() == Camera::MoveState::Shift_Left)return;

	switch (nowPoint)
	{
	case Point::L_Point5:
		nowPoint = Point::L_Point4;
		break;
	case Point::L_Point4:
		nowPoint = Point::L_Point3;
		break;
	case Point::L_Point3:
		nowPoint = Point::L_Point2;
		break;
	case Point::L_Point2:
		nowPoint = Point::L_Point1;
		break;
	case Point::L_Point1:
//		nowPoint = Point::L_Point4;
		switch (type)
		{
		case Enemy::EnemyType::TotemPole:
			BaseScene::lastEnemyType = 2;
			break;
		case Enemy::EnemyType::Ghost:
			BaseScene::lastEnemyType = 1;
			break;
		case Enemy::EnemyType::Daruma:
			BaseScene::lastEnemyType = 0;
			break;
		default:
			break;
		}
		SceneManager::Get().SetScene(SceneManager::GAME_OVER);
		break;
	}

}

void Enemy::MoveCross()
{
	if (Camera::Get().GetCanPushSwitch() && Camera::Get().GetMoveState() == Camera::MoveState::Shift_Left)return;

	switch (nowPoint)
	{
	case Enemy::R_Point1:
		switch (type)
		{
		case Enemy::EnemyType::TotemPole:
			BaseScene::lastEnemyType = 2;
			break;
		case Enemy::EnemyType::Ghost:
			BaseScene::lastEnemyType = 1;
			break;
		case Enemy::EnemyType::Daruma:
			BaseScene::lastEnemyType = 0;
			break;
		default:
			break;
		}
		SceneManager::Get().SetScene(SceneManager::GAME_OVER);

		break;
	case Enemy::R_Point2:
		nowPoint = Enemy::L_Point1;
		break;
	case Enemy::R_Point3:
		nowPoint = Enemy::L_Point2;
		break;
	case Enemy::R_Point4:
		nowPoint = Enemy::L_Point3;
		break;
	case Enemy::R_Point5:
		nowPoint = Enemy::L_Point4;
		break;
	case Enemy::L_Point1:
		switch (type)
		{
		case Enemy::EnemyType::TotemPole:
			BaseScene::lastEnemyType = 2;
			break;
		case Enemy::EnemyType::Ghost:
			BaseScene::lastEnemyType = 1;
			break;
		case Enemy::EnemyType::Daruma:
			BaseScene::lastEnemyType = 0;
			break;
		default:
			break;
		}
		SceneManager::Get().SetScene(SceneManager::GAME_OVER);

		break;
	case Enemy::L_Point2:
		nowPoint = Enemy::R_Point1;
		break;
	case Enemy::L_Point3:
		nowPoint = Enemy::R_Point2;
		break;
	case Enemy::L_Point4:
		nowPoint = Enemy::R_Point3;
		break;
	case Enemy::L_Point5:
		nowPoint = Enemy::R_Point4;
		break;
	default:
		break;
	}
}

/*------------------------------------*/
//	自身の消去関数
/*------------------------------------*/
void Enemy::ErasePreparation()
{
	isEnable = false;
}

/*------------------------------------*/
//	自身の消去関数
/*------------------------------------*/
void Enemy::EraseThisInstance()
{
	if (++eraseCount >= ERASE_COUNT)
	{
		ErasePreparation();
	}

	if (eraseCount % 4 >= 2)
	{
		pos.x += 0.5f;
	}
	else
	{
		pos.x -= 0.5f;
	}
}