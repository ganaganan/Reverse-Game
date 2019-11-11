#include "Enemy.h"

#include <framework.h>

#include "FilePath.h"
#include "Light.h"
#include "Camera.h"

/*------------------------------------*/
//	�e�|�C���g�̈ʒu���
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
//	Manager�̏�����
/*------------------------------------*/
void EnemyManager::Init()
{
//	totemPoleModel = std::make_unique<SkinnedMesh>(FRAMEWORK->GetDevice(), GetModelPath(ModelAttribute::TotemPole));
	ghostModel = std::make_unique<SkinnedMesh>(FRAMEWORK->GetDevice(), GetModelPath(ModelAttribute::Ghost));

	for (auto& it : enemy)
	{
		it.Init();
	}
}

/*------------------------------------*/
//	Manager�̍X�V
/*------------------------------------*/
void EnemyManager::Update()
{
	for (auto& it : enemy)
	{
		it.Update();
	}
#ifdef USE_IMGUI
	ImGui::Begin("enemy pos");
	ImGui::SliderFloat("x", &enemy[0].pos.x, -100.0f, 100.0f);
	ImGui::SliderFloat("y", &enemy[0].pos.y, -100.0f, 100.0f);
	ImGui::SliderFloat("z", &enemy[0].pos.z, -100.0f, 500.0f);
	ImGui::NewLine();
	ImGui::SliderFloat("x", &enemy[1].pos.x, -100.0f, 100.0f);
	ImGui::SliderFloat("y", &enemy[1].pos.y, -100.0f, 100.0f);
	ImGui::SliderFloat("z", &enemy[1].pos.z, -100.0f, 500.0f);

	ImGui::End();
#endif
}

/*------------------------------------*/
//	Manager�̕`��
/*------------------------------------*/
void EnemyManager::Render(DirectX::XMMATRIX& _V, DirectX::XMMATRIX& _P)
{
	for (auto& it : enemy)
	{
		it.Render(_V, _P);
	}
}

/*------------------------------------*/
//	Manager�̏I������
/*------------------------------------*/
void EnemyManager::Uninit()
{
	for (auto& it : enemy)
	{
		it.Uninit();
	}
}

/*------------------------------------*/
//	�G�̐���
/*------------------------------------*/
void EnemyManager::GenerateEnemy(
	DirectX::XMFLOAT3 _scale,
	Enemy::EnemyType _type, Enemy::Point _generatePoint)
{
	for (auto& it : enemy)
	{
		if (it.GetEnable())continue;

		it.Generate(
			_scale,
			_type, _generatePoint
		);
		break;
	}
}


/*------------------------------------*/
//	Enemy�̏�����
/*------------------------------------*/
void Enemy::Init()
{
	pos = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
	scale = DirectX::XMFLOAT3(0.1f, 0.1f, 0.1f);
	type = EnemyType::TotemPole;
	nowPoint = Point::R_Point1;
	moveTime = 0;
	isEnable = false;
	isPreparationErase = false;
	isHitLight = false;
}

/*------------------------------------*/
//	Enemy�̍X�V
/*------------------------------------*/
void Enemy::Update()
{
	if (!isEnable)return;

	Move();

	JudgeIsHitLight();
}

/*------------------------------------*/
//	Enemy�̕`��
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
	default:
		break;
	}
}

/*------------------------------------*/
//	Enemy�̏I������
/*------------------------------------*/
void Enemy::Uninit()
{

}

/*------------------------------------*/
//	Enemy�̈ړ��֐�
/*------------------------------------*/
void Enemy::Move()
{
	moveTime++;

	if (moveTime >= ENEMY_WAIT_COUNT)
	{
		if (nowPoint>= Point::R_Point1 && Point::R_Point5>=nowPoint)
		{
			MoveRightPoint();
		}
		else
		{
			MoveLeftPoint();

		}
		pos = pointPosition[nowPoint];
		moveTime = 0;
	}
}

/*------------------------------------*/
//	Enemy�̐���
/*------------------------------------*/
void Enemy::Generate(
	DirectX::XMFLOAT3 _scale,
	EnemyType _type, Point _generatePoint)
{
	pos = pointPosition[_generatePoint];
	scale = _scale;
	type = _type;
	nowPoint = _generatePoint;
	isEnable = true;
	isPreparationErase = false;
	isHitLight = false;
}

/*------------------------------------*/
//	���C�g�ɓ������Ă��邩��Ԃ�
/*------------------------------------*/
bool Enemy::JudgeIsHitLight()
{
	switch (nowPoint)
	{
	case Point::R_Point1:
	case Point::R_Point2:
		if (Light::JudgeLightEnable(1))
		{
			// ���C�g�ɂ������Ă���
			if (!isPreparationErase)
			{
				isPreparationErase = true;
				EraseThisInstance();
			}
		}
		break;
	case Point::L_Point1:
	case Point::L_Point2:
		if (Light::JudgeLightEnable(0))
		{
			// ���C�g�ɂ������Ă���
			if (!isPreparationErase)
			{
				isPreparationErase = true;
				EraseThisInstance();
			}
		}
		break;

	default:
		break;
	}
	return true;
}

/*------------------------------------*/
//	�E���̈ړ��֐�
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
		nowPoint = Point::R_Point4;
		break;
	}
}

/*------------------------------------*/
//	�����̈ړ��֐�
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
		nowPoint = Point::L_Point4;
		break;
	}

}

/*------------------------------------*/
//	���g�̏����֐�
/*------------------------------------*/
void Enemy::ErasePreparation()
{

}

/*------------------------------------*/
//	���g�̏����֐�
/*------------------------------------*/
void Enemy::EraseThisInstance()
{

}