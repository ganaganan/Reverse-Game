#include "SceneGame.h"

#include <DirectXMath.h>
#include <framework.h>

#include "Camera.h"
#include "Stage.h"
#include "Enemy.h"
#include "Light.h"
#include "Sound.h"

#pragma region Day1
void SceneGame::Init()
{
	// エネミーの生成間隔
	GENERATE_ENEMY_MIN = 300;
	GENERATE_ENEMY_MAX = 500;
	// 敵ごとの生成確率
	GENERATE_PER_ENEMY1 = 70;
	GENERATE_PER_ENEMY2 = 30;
	GENERATE_PER_ENEMY3 = 0;
	generateEnemyCount = 0;
	thisTimeGeneratePerCount = 90;

	nightTime = 0;

	nowDay = 0;

	// デバッグ用
	debugLightPos[0] = DirectX::XMFLOAT3(-38.0f, 25.0f, 75.0f);
	debugLightPos[1] = DirectX::XMFLOAT3(38.0f, 25.0f, 75.0f);
	debugLightPos[2] = DirectX::XMFLOAT3(0.0f, 60.0f, 2.0f);
	debugLightPos[3] = DirectX::XMFLOAT3(-100.0f, 10.0f, 85.0f);
	debugLightPos[4] = DirectX::XMFLOAT3(100.0f, 10.0f, 85.0f);


	debugLightRange[0] = 100.0f;
	debugLightRange[1] = 100.0f;
	debugLightRange[2] = 160.0f;
	debugLightRange[3] = 0.0f;
	debugLightRange[4] = 0.0f;


	// コンスタントバッファ作成
	CreateConstantBuffer(&constantBuffer, sizeof(LightConstantBuffer));

	Stage::Get().Init();
	EnemyManager::Get().Init();
	Sound::Get().Init();

	// ライトの設定
	Light::Init();
	Light::SetAmbient(DirectX::XMFLOAT3(0.1f, 0.1f, 0.1f));

	lightDir.x = sinf(DirectX::XM_PI);
	lightDir.y = -0.4f;
	lightDir.z = cosf(DirectX::XM_PI);
	Light::SetDirLight(lightDir, DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f));

	Light::SetPointLight(0, debugLightPos[0]/*DirectX::XMFLOAT3(-50.0f, -7.0f, 27.0f)*/, DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f), debugLightRange[0]);	// 左
	Light::SetPointLight(1, debugLightPos[1]/*DirectX::XMFLOAT3(50.0f, -7.0f, 27.0f)*/, DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f), debugLightRange[1]);	// 右
	Light::SetPointLight(2, debugLightPos[2]/*DirectX::XMFLOAT3(0.0f, -26.0f, 63.0f)*/, DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f), debugLightRange[2]);	// 真ん中
	Light::TurnOffPointLight(0);
	Light::TurnOffPointLight(1);
	Light::amountState = Light::ConsumptionAmount::Small;


	// Set Sound
	sound[SoundType::Chaim] = new Audio("Data/Sound/チャイム.wav");
	sound[SoundType::OpenDoor] = new Audio("Data/Sound/ドアの音.wav");
	sound[SoundType::Rain] = new Audio("Data/Sound/雨.wav");
	sound[SoundType::Pass] = new Audio("Data/Sound/通りゃんせ.wav");
	sound[SoundType::RingTone] = new Audio("Data/Sound/電話.wav");
	sound[SoundType::Wind] = new Audio("Data/Sound/風で木の葉が揺れる音.wav");
	sound[SoundType::Walk] = new Audio("Data/Sound/歩く音ブーツ.wav");
	sound[SoundType::Thunder] = new Audio("Data/Sound/雷.wav");

	PLAY_SOUND_MAX = 300;
	PLAY_SOUND_MIN = 180;
	SOUND_VOLUME_MAX = 1.0f;
	SOUND_VOLUME_MIN = 0.5f;
	int tmp = PLAY_SOUND_MAX - PLAY_SOUND_MIN;
	thisTimePlaySound = rand() % tmp + PLAY_SOUND_MIN;
	tmp = SOUND_VOLUME_MAX - SOUND_VOLUME_MIN;
	SoundVol = rand() % tmp + SOUND_VOLUME_MIN;

	for (auto& it : soundVol)it = 1.0f;
}

void SceneGame::Update()
{	
#ifdef USE_IMGUI
	UseImGui();
#endif
	// ポイントライトの設定
	Light::SetPointLight(2, debugLightPos[2]/*DirectX::XMFLOAT3(0.0f, -26.0f, 63.0f)*/,		DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f), debugLightRange[2]);	// 真ん中
//	Light::SetPointLight(3, debugLightPos[3]/*DirectX::XMFLOAT3(0.0f, -26.0f, 63.0f)*/,		DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f), debugLightRange[3]);	// 左中
//	Light::SetPointLight(4, debugLightPos[4]/*DirectX::XMFLOAT3(0.0f, -26.0f, 63.0f)*/,		DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f), debugLightRange[4]);	// 右中

	Light::Update();
	Camera::Get().Update();
	Stage::Get().Update();
	EnemyManager::Get().Update();
	Sound::Get().Update();

	GenerateEnemy();

	JudgeGoNextDay();

	PlayEnvironment();

	nightTime++;
}

void SceneGame::Render()
{
	using namespace DirectX;

	// 行列設定
	XMMATRIX V, P;
	V =	Camera::Get().GetViewMatrix();
	P =	Camera::Get().GetProjectionMatrix();

	// 定数バッファの設定
	SetConstantBuffer();

	Stage::Get().Render(V, P);
	EnemyManager::Get().Render(V, P);
}

void SceneGame::Uninit()
{
	for (auto& it : sound)
	{
		delete it;
		it = nullptr;
	}
}

#ifdef USE_IMGUI
void SceneGame::UseImGui()
{
	ImGui::Begin("SceneGame Day1");

	if (ImGui::TreeNode("PointLight Status"))
	{
		if (ImGui::TreeNode(" No.1 "))
		{
			ImGui::SliderFloat3("Position ", &debugLightPos[0].x, -100.0f, 100.0f);
			ImGui::SliderFloat("Range ", &debugLightRange[0], 0.1f, 1000.0f);
			ImGui::TreePop();
		}
		if (ImGui::TreeNode(" No.2 "))
		{
			ImGui::SliderFloat3("Position ", &debugLightPos[1].x, -100.0f, 100.0f);
			ImGui::SliderFloat("Range ", &debugLightRange[1], 0.1f, 1000.0f);
			ImGui::TreePop();
		}
		if (ImGui::TreeNode(" No.3 "))
		{
			ImGui::SliderFloat3("Position ", &debugLightPos[2].x, -100.0f, 100.0f);
			ImGui::SliderFloat("Range ", &debugLightRange[2], 0.1f, 1000.0f);
			ImGui::TreePop();
		}
		if (ImGui::TreeNode(" No.4 "))
		{
			ImGui::SliderFloat3("Position ", &debugLightPos[3].x, -100.0f, 100.0f);
			ImGui::SliderFloat("Range ", &debugLightRange[3], 0.1f, 1000.0f);
			ImGui::TreePop();
		}
		if (ImGui::TreeNode(" No.5 "))
		{
			ImGui::SliderFloat3("Position ", &debugLightPos[4].x, -200.0f, 200.0f);
			ImGui::SliderFloat("Range ", &debugLightRange[4], 0.1f, 1000.0f);
			ImGui::TreePop();
		}


		ImGui::TreePop();
	}

	if (ImGui::TreeNode("Generate Enemy Parameter"))
	{
		ImGui::InputInt("GENERATE_PER_ENEMY1", &GENERATE_PER_ENEMY1);
		ImGui::InputInt("GENERATE_PER_ENEMY2", &GENERATE_PER_ENEMY2);
		ImGui::InputInt("GENERATE_PER_ENEMY3", &GENERATE_PER_ENEMY3);

		ImGui::InputInt("GENERATE_ENEMY_MAX", &GENERATE_ENEMY_MAX);
		ImGui::InputInt("GENERATE_ENEMY_MIN", &GENERATE_ENEMY_MIN);
		ImGui::TreePop();
	}

	if (ImGui::TreeNode(" Sound Volume of Environment "))
	{
		ImGui::SliderFloat(" Chaim", &soundVol[SoundType::Chaim], 0.0f, 1.0f);
		ImGui::SliderFloat(" OpenDoor", &soundVol[SoundType::OpenDoor], 0.0f, 1.0f);
		ImGui::SliderFloat(" Rain", &soundVol[SoundType::Rain], 0.0f, 1.0f);
		ImGui::SliderFloat(" Pass", &soundVol[SoundType::Pass], 0.0f, 1.0f);
		ImGui::SliderFloat(" RingTone", &soundVol[SoundType::RingTone], 0.0f, 1.0f);
		ImGui::SliderFloat(" Wind", &soundVol[SoundType::Wind], 0.0f, 1.0f);
		ImGui::SliderFloat(" Walk", &soundVol[SoundType::Walk], 0.0f, 1.0f);
		ImGui::SliderFloat(" Thunder", &soundVol[SoundType::Thunder], 0.0f, 1.0f);
		ImGui::TreePop();
	}

	ImGui::Text(" nightTime : %d", nightTime);
	ImGui::InputInt("END_OF_NIGHT", &TOTAL_NIGHTS);
	ImGui::End();

}
#endif

void SceneGame::GenerateEnemy()
{
	generateEnemyCount++;

	if (generateEnemyCount >= thisTimeGeneratePerCount)
	{
		int generateEnemy1Per = 100 - GENERATE_PER_ENEMY1;
		int generateEnemy2Per = 100 - GENERATE_PER_ENEMY2;
		int generateEnemy3Per = 100 - GENERATE_PER_ENEMY3;

		if (rand() % 200 >= 100)
		{
			if (rand() % 100 >= generateEnemy1Per)
			{
				EnemyManager::Get().GenerateEnemy(Enemy::EnemyType::Daruma, Enemy::Point::R_Point5);
			}
			else if (rand() % 100 >= generateEnemy2Per)
			{
				EnemyManager::Get().GenerateEnemy(Enemy::EnemyType::Ghost, Enemy::Point::R_Point5);
			}
			else if (rand() % 100 >= generateEnemy3Per)
			{
				EnemyManager::Get().GenerateEnemy(Enemy::EnemyType::TotemPole, Enemy::Point::R_Point5);
			}
		}
		else
		{
			if (rand() % 100 >= generateEnemy1Per)
			{
				EnemyManager::Get().GenerateEnemy(Enemy::EnemyType::Daruma, Enemy::Point::L_Point5);
			}
			else if (rand() % 100 >= generateEnemy2Per)
			{
				EnemyManager::Get().GenerateEnemy(Enemy::EnemyType::Ghost, Enemy::Point::L_Point5);
			}
			else if (rand() % 100 >= generateEnemy3Per)
			{
				EnemyManager::Get().GenerateEnemy(Enemy::EnemyType::TotemPole, Enemy::Point::L_Point5);
			}

		}
		int tmp = GENERATE_ENEMY_MAX - GENERATE_ENEMY_MIN;
		thisTimeGeneratePerCount = rand() % tmp + GENERATE_ENEMY_MIN;
		generateEnemyCount = 0;
	}
}

void SceneGame::JudgeGoNextDay()
{
	if (nightTime >= TOTAL_NIGHTS)
	{
		SceneManager::Get().SetScene(SceneManager::MAIN_GAME2);
	}
}

void SceneGame::PlayEnvironment()
{
	static int timer;
	if (++timer >= thisTimePlaySound)
	{
		float tmp = SOUND_VOLUME_MAX - SOUND_VOLUME_MIN;
		int tmp_i = 100 * tmp;
		float tmpVol = static_cast<float>(rand() % tmp_i + SOUND_VOLUME_MIN * 100) / 100;
		if (rand() % 200 >= 100)
		{
			// 右
			int rnd = rand() % SoundType::Max;
			switch (rnd)
			{
			case 0:		// Chaim
				sound[SoundType::Chaim]->SetPosition(DirectX::XMFLOAT3( 38.0f, 0.0f, 0.0f));
				sound[SoundType::Chaim]->SetVolume(tmpVol);
				sound[SoundType::Chaim]->Play(false);
				break;
			case 1:		// OpenDoor
				sound[SoundType::OpenDoor]->SetPosition(DirectX::XMFLOAT3(38.0f, 0.0f, 0.0f));
				sound[SoundType::OpenDoor]->SetVolume(tmpVol);
				sound[SoundType::OpenDoor]->Play(false);
				break;
			case 2:		// Rain
				sound[SoundType::Rain]->SetPosition(DirectX::XMFLOAT3(38.0f, 0.0f, 0.0f));
				sound[SoundType::Rain]->SetVolume(tmpVol);
				sound[SoundType::Rain]->Play(false);
				break;
			case 3:		// Pass
				sound[SoundType::Pass]->SetPosition(DirectX::XMFLOAT3(38.0f, 0.0f, 0.0f));
				sound[SoundType::Pass]->Play(false);
				break;
			case 4:		// RingTone
				sound[SoundType::RingTone]->SetPosition(DirectX::XMFLOAT3(38.0f, 0.0f, 0.0f));
				sound[SoundType::RingTone]->SetVolume(tmpVol);
				sound[SoundType::RingTone]->Play(false);
				break;
			case 5:		// Wind
				sound[SoundType::Wind]->SetPosition(DirectX::XMFLOAT3(38.0f, 0.0f, 0.0f));
				sound[SoundType::Wind]->SetVolume(tmpVol);
				sound[SoundType::Wind]->Play(false);
				break;
			case 6:		// Walk
				sound[SoundType::Walk]->SetPosition(DirectX::XMFLOAT3(38.0f, 0.0f, 0.0f));
				sound[SoundType::Walk]->SetVolume(tmpVol);
				sound[SoundType::Walk]->Play(false);
				break;
			case 7:		// Thunder
				sound[SoundType::Thunder]->SetPosition(DirectX::XMFLOAT3(38.0f, 0.0f, 0.0f));
				sound[SoundType::Thunder]->SetVolume(tmpVol);
				sound[SoundType::Thunder]->Play(false);

				break;
			}
		}
		else
		{
			// 左
			int rnd = rand() % SoundType::Max;
			switch (rnd)
			{
			case 0:		// Chaim
				sound[SoundType::Chaim]->SetPosition(DirectX::XMFLOAT3(-38.0f, 0.0f, 0.0f));
				sound[SoundType::Chaim]->SetVolume(tmpVol);
				sound[SoundType::Chaim]->Play(false);
				break;
			case 1:		// OpenDoor
				sound[SoundType::OpenDoor]->SetPosition(DirectX::XMFLOAT3(-38.0f, 0.0f, 0.0f));
				sound[SoundType::OpenDoor]->SetVolume(tmpVol);
				sound[SoundType::OpenDoor]->Play(false);
				break;
			case 2:		// Rain
				sound[SoundType::Rain]->SetPosition(DirectX::XMFLOAT3(-38.0f, 0.0f, 0.0f));
				sound[SoundType::Rain]->SetVolume(tmpVol);
				sound[SoundType::Rain]->Play(false);
				break;
			case 3:		// Pass
				sound[SoundType::Pass]->SetPosition(DirectX::XMFLOAT3(-38.0f, 0.0f, 0.0f));
				sound[SoundType::Pass]->Play(false);
				break;
			case 4:		// RingTone
				sound[SoundType::RingTone]->SetPosition(DirectX::XMFLOAT3(-38.0f, 0.0f, 0.0f));
				sound[SoundType::RingTone]->SetVolume(tmpVol);
				sound[SoundType::RingTone]->Play(false);
				break;
			case 5:		// Wind
				sound[SoundType::Wind]->SetPosition(DirectX::XMFLOAT3(-38.0f, 0.0f, 0.0f));
				sound[SoundType::Wind]->SetVolume(tmpVol);
				sound[SoundType::Wind]->Play(false);
				break;
			case 6:		// Walk
				sound[SoundType::Walk]->SetPosition(DirectX::XMFLOAT3(-38.0f, 0.0f, 0.0f));
				sound[SoundType::Walk]->SetVolume(tmpVol);
				sound[SoundType::Walk]->Play(false);
				break;
			case 7:		// Thunder
				sound[SoundType::Thunder]->SetPosition(DirectX::XMFLOAT3(-38.0f, 0.0f, 0.0f));
				sound[SoundType::Thunder]->SetVolume(tmpVol);
				sound[SoundType::Thunder]->Play(false);

				break;
			}
		}

		int pre = PLAY_SOUND_MAX - PLAY_SOUND_MIN;
		thisTimePlaySound = rand() % pre + PLAY_SOUND_MIN;
		timer = 0;
	}
}

#pragma endregion


#pragma region Day2
void SceneGame2::Init()
{
	// エネミーの生成間隔
	GENERATE_ENEMY_MIN = 300;
	GENERATE_ENEMY_MAX = 500;
	// 敵ごとの生成確率
	GENERATE_PER_ENEMY1 = 70;
	GENERATE_PER_ENEMY2 = 30;
	GENERATE_PER_ENEMY3 = 0;
	generateEnemyCount = 0;
	thisTimeGeneratePerCount = 90;

	nightTime = 0;

	nowDay = 1;

	// デバッグ用
	debugLightPos[0] = DirectX::XMFLOAT3(-38.0f, 25.0f, 75.0f);
	debugLightPos[1] = DirectX::XMFLOAT3(38.0f, 25.0f, 75.0f);
	debugLightPos[2] = DirectX::XMFLOAT3(0.0f, 60.0f, 2.0f);
	debugLightPos[3] = DirectX::XMFLOAT3(-100.0f, 10.0f, 85.0f);
	debugLightPos[4] = DirectX::XMFLOAT3(100.0f, 10.0f, 85.0f);


	debugLightRange[0] = 100.0f;
	debugLightRange[1] = 100.0f;
	debugLightRange[2] = 160.0f;
	debugLightRange[3] = 0.0f;
	debugLightRange[4] = 0.0f;


	// コンスタントバッファ作成
	CreateConstantBuffer(&constantBuffer, sizeof(LightConstantBuffer));

	Stage::Get().Init();
	EnemyManager::Get().Init();
	Sound::Get().Init();

	// ライトの設定
	Light::Init();
	Light::SetAmbient(DirectX::XMFLOAT3(0.1f, 0.1f, 0.1f));

	lightDir.x = sinf(DirectX::XM_PI);
	lightDir.y = -0.4f;
	lightDir.z = cosf(DirectX::XM_PI);
	Light::SetDirLight(lightDir, DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f));

	Light::SetPointLight(0, debugLightPos[0]/*DirectX::XMFLOAT3(-50.0f, -7.0f, 27.0f)*/, DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f), debugLightRange[0]);	// 左
	Light::SetPointLight(1, debugLightPos[1]/*DirectX::XMFLOAT3(50.0f, -7.0f, 27.0f)*/, DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f), debugLightRange[1]);	// 右
	Light::SetPointLight(2, debugLightPos[2]/*DirectX::XMFLOAT3(0.0f, -26.0f, 63.0f)*/, DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f), debugLightRange[2]);	// 真ん中
	Light::TurnOffPointLight(0);
	Light::TurnOffPointLight(1);
	Light::amountState = Light::ConsumptionAmount::Small;

	sound[SoundType::Chaim] = new Audio("Data/Sound/チャイム.wav");
	sound[SoundType::OpenDoor] = new Audio("Data/Sound/ドアの音.wav");
	sound[SoundType::Rain] = new Audio("Data/Sound/雨.wav");
	sound[SoundType::Pass] = new Audio("Data/Sound/通りゃんせ.wav");
	sound[SoundType::RingTone] = new Audio("Data/Sound/電話.wav");
	sound[SoundType::Wind] = new Audio("Data/Sound/風で木の葉が揺れる音.wav");
	sound[SoundType::Walk] = new Audio("Data/Sound/歩く音ブーツ.wav");
	sound[SoundType::Thunder] = new Audio("Data/Sound/雷.wav");

	PLAY_SOUND_MAX = 300;
	PLAY_SOUND_MIN = 180;
	SOUND_VOLUME_MAX = 1.0f;
	SOUND_VOLUME_MIN = 0.5f;
	int tmp = PLAY_SOUND_MAX - PLAY_SOUND_MIN;
	thisTimePlaySound = rand() % tmp + PLAY_SOUND_MIN;
	tmp = SOUND_VOLUME_MAX - SOUND_VOLUME_MIN;
	SoundVol = rand() % tmp + SOUND_VOLUME_MIN;

	for (auto& it : soundVol)it = 1.0f;

}

void SceneGame2::Update()
{
#ifdef USE_IMGUI
	UseImGui();
#endif
	// ポイントライトの設定
	Light::SetPointLight(2, debugLightPos[2], DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f), debugLightRange[2]);	// 真ん中

	Light::Update();
	Camera::Get().Update();
	Stage::Get().Update();
	EnemyManager::Get().Update();
	Sound::Get().Update();

	GenerateEnemy();
	JudgeGoNextDay();
	PlayEnvironment();

	nightTime++;
}

void SceneGame2::Render()
{
	using namespace DirectX;

	// 行列設定
	XMMATRIX V, P;
	V = Camera::Get().GetViewMatrix();
	P = Camera::Get().GetProjectionMatrix();

	// 定数バッファの設定
	SetConstantBuffer();

	Stage::Get().Render(V, P);
	EnemyManager::Get().Render(V, P);
}

void SceneGame2::Uninit()
{

}

#ifdef USE_IMGUI
void SceneGame2::UseImGui()
{
	ImGui::Begin("SceneGame Day2");

	if (ImGui::TreeNode("PointLight Status"))
	{
		if (ImGui::TreeNode(" No.1 "))
		{
			ImGui::SliderFloat3("Position ", &debugLightPos[0].x, -100.0f, 100.0f);
			ImGui::SliderFloat("Range ", &debugLightRange[0], 0.1f, 1000.0f);
			ImGui::TreePop();
		}
		if (ImGui::TreeNode(" No.2 "))
		{
			ImGui::SliderFloat3("Position ", &debugLightPos[1].x, -100.0f, 100.0f);
			ImGui::SliderFloat("Range ", &debugLightRange[1], 0.1f, 1000.0f);
			ImGui::TreePop();
		}
		if (ImGui::TreeNode(" No.3 "))
		{
			ImGui::SliderFloat3("Position ", &debugLightPos[2].x, -100.0f, 100.0f);
			ImGui::SliderFloat("Range ", &debugLightRange[2], 0.1f, 1000.0f);
			ImGui::TreePop();
		}
		if (ImGui::TreeNode(" No.4 "))
		{
			ImGui::SliderFloat3("Position ", &debugLightPos[3].x, -100.0f, 100.0f);
			ImGui::SliderFloat("Range ", &debugLightRange[3], 0.1f, 1000.0f);
			ImGui::TreePop();
		}
		if (ImGui::TreeNode(" No.5 "))
		{
			ImGui::SliderFloat3("Position ", &debugLightPos[4].x, -200.0f, 200.0f);
			ImGui::SliderFloat("Range ", &debugLightRange[4], 0.1f, 1000.0f);
			ImGui::TreePop();
		}


		ImGui::TreePop();
	}

	if (ImGui::TreeNode("Generate Enemy Parameter"))
	{
		ImGui::InputInt("GENERATE_PER_ENEMY1", &GENERATE_PER_ENEMY1);
		ImGui::InputInt("GENERATE_PER_ENEMY2", &GENERATE_PER_ENEMY2);
		ImGui::InputInt("GENERATE_PER_ENEMY3", &GENERATE_PER_ENEMY3);

		ImGui::InputInt("GENERATE_ENEMY_MAX", &GENERATE_ENEMY_MAX);
		ImGui::InputInt("GENERATE_ENEMY_MIN", &GENERATE_ENEMY_MIN);
		ImGui::TreePop();
	}

	ImGui::Text(" nightTime : %d", nightTime);
	ImGui::InputInt("END_OF_NIGHT", &TOTAL_NIGHTS);

	ImGui::End();

}
#endif

void SceneGame2::GenerateEnemy()
{
	generateEnemyCount++;

	if (generateEnemyCount >= thisTimeGeneratePerCount)
	{
		int generateEnemy1Per = 100 - GENERATE_PER_ENEMY1;
		int generateEnemy2Per = 100 - GENERATE_PER_ENEMY2;
		int generateEnemy3Per = 100 - GENERATE_PER_ENEMY3;

		if (rand() % 200 >= 100)
		{
			if (rand() % 100 >= generateEnemy1Per)
			{
				EnemyManager::Get().GenerateEnemy(Enemy::EnemyType::Daruma, Enemy::Point::R_Point5);
			}
			else if (rand() % 100 >= generateEnemy2Per)
			{
				EnemyManager::Get().GenerateEnemy(Enemy::EnemyType::Ghost, Enemy::Point::R_Point5);
			}
			else if (rand() % 100 >= generateEnemy3Per)
			{
				EnemyManager::Get().GenerateEnemy(Enemy::EnemyType::TotemPole, Enemy::Point::R_Point5);
			}
		}
		else
		{
			if (rand() % 100 >= generateEnemy1Per)
			{
				EnemyManager::Get().GenerateEnemy(Enemy::EnemyType::Daruma, Enemy::Point::L_Point5);
			}
			else if (rand() % 100 >= generateEnemy2Per)
			{
				EnemyManager::Get().GenerateEnemy(Enemy::EnemyType::Ghost, Enemy::Point::L_Point5);
			}
			else if (rand() % 100 >= generateEnemy3Per)
			{
				EnemyManager::Get().GenerateEnemy(Enemy::EnemyType::TotemPole, Enemy::Point::L_Point5);
			}

		}
		int tmp = GENERATE_ENEMY_MAX - GENERATE_ENEMY_MIN;
		thisTimeGeneratePerCount = rand() % tmp + GENERATE_ENEMY_MIN;
		generateEnemyCount = 0;
	}
}

void SceneGame2::JudgeGoNextDay()
{
	if (nightTime >= TOTAL_NIGHTS)
	{
		SceneManager::Get().SetScene(SceneManager::MAIN_GAME3);
	}

}

void SceneGame2::PlayEnvironment()
{
	static int timer;
	if (++timer >= thisTimePlaySound)
	{
		float tmp = SOUND_VOLUME_MAX - SOUND_VOLUME_MIN;
		int tmp_i = 100 * tmp;
		float tmpVol = static_cast<float>(rand() % tmp_i + SOUND_VOLUME_MIN * 100) / 100;
		if (rand() % 200 >= 100)
		{
			// 右
			int rnd = rand() % SoundType::Max;
			switch (rnd)
			{
			case 0:		// Chaim
				sound[SoundType::Chaim]->SetPosition(DirectX::XMFLOAT3(38.0f, 0.0f, 0.0f));
				sound[SoundType::Chaim]->SetVolume(tmpVol);
				sound[SoundType::Chaim]->Play(false);
				break;
			case 1:		// OpenDoor
				sound[SoundType::OpenDoor]->SetPosition(DirectX::XMFLOAT3(38.0f, 0.0f, 0.0f));
				sound[SoundType::OpenDoor]->SetVolume(tmpVol);
				sound[SoundType::OpenDoor]->Play(false);
				break;
			case 2:		// Rain
				sound[SoundType::Rain]->SetPosition(DirectX::XMFLOAT3(38.0f, 0.0f, 0.0f));
				sound[SoundType::Rain]->SetVolume(tmpVol);
				sound[SoundType::Rain]->Play(false);
				break;
			case 3:		// Pass
				sound[SoundType::Pass]->SetPosition(DirectX::XMFLOAT3(38.0f, 0.0f, 0.0f));
				sound[SoundType::Pass]->Play(false);
				break;
			case 4:		// RingTone
				sound[SoundType::RingTone]->SetPosition(DirectX::XMFLOAT3(38.0f, 0.0f, 0.0f));
				sound[SoundType::RingTone]->SetVolume(tmpVol);
				sound[SoundType::RingTone]->Play(false);
				break;
			case 5:		// Wind
				sound[SoundType::Wind]->SetPosition(DirectX::XMFLOAT3(38.0f, 0.0f, 0.0f));
				sound[SoundType::Wind]->SetVolume(tmpVol);
				sound[SoundType::Wind]->Play(false);
				break;
			case 6:		// Walk
				sound[SoundType::Walk]->SetPosition(DirectX::XMFLOAT3(38.0f, 0.0f, 0.0f));
				sound[SoundType::Walk]->SetVolume(tmpVol);
				sound[SoundType::Walk]->Play(false);
				break;
			case 7:		// Thunder
				sound[SoundType::Thunder]->SetPosition(DirectX::XMFLOAT3(38.0f, 0.0f, 0.0f));
				sound[SoundType::Thunder]->SetVolume(tmpVol);
				sound[SoundType::Thunder]->Play(false);

				break;
			}
		}
		else
		{
			// 左
			int rnd = rand() % SoundType::Max;
			switch (rnd)
			{
			case 0:		// Chaim
				sound[SoundType::Chaim]->SetPosition(DirectX::XMFLOAT3(-38.0f, 0.0f, 0.0f));
				sound[SoundType::Chaim]->SetVolume(tmpVol);
				sound[SoundType::Chaim]->Play(false);
				break;
			case 1:		// OpenDoor
				sound[SoundType::OpenDoor]->SetPosition(DirectX::XMFLOAT3(-38.0f, 0.0f, 0.0f));
				sound[SoundType::OpenDoor]->SetVolume(tmpVol);
				sound[SoundType::OpenDoor]->Play(false);
				break;
			case 2:		// Rain
				sound[SoundType::Rain]->SetPosition(DirectX::XMFLOAT3(-38.0f, 0.0f, 0.0f));
				sound[SoundType::Rain]->SetVolume(tmpVol);
				sound[SoundType::Rain]->Play(false);
				break;
			case 3:		// Pass
				sound[SoundType::Pass]->SetPosition(DirectX::XMFLOAT3(-38.0f, 0.0f, 0.0f));
				sound[SoundType::Pass]->Play(false);
				break;
			case 4:		// RingTone
				sound[SoundType::RingTone]->SetPosition(DirectX::XMFLOAT3(-38.0f, 0.0f, 0.0f));
				sound[SoundType::RingTone]->SetVolume(tmpVol);
				sound[SoundType::RingTone]->Play(false);
				break;
			case 5:		// Wind
				sound[SoundType::Wind]->SetPosition(DirectX::XMFLOAT3(-38.0f, 0.0f, 0.0f));
				sound[SoundType::Wind]->SetVolume(tmpVol);
				sound[SoundType::Wind]->Play(false);
				break;
			case 6:		// Walk
				sound[SoundType::Walk]->SetPosition(DirectX::XMFLOAT3(-38.0f, 0.0f, 0.0f));
				sound[SoundType::Walk]->SetVolume(tmpVol);
				sound[SoundType::Walk]->Play(false);
				break;
			case 7:		// Thunder
				sound[SoundType::Thunder]->SetPosition(DirectX::XMFLOAT3(-38.0f, 0.0f, 0.0f));
				sound[SoundType::Thunder]->SetVolume(tmpVol);
				sound[SoundType::Thunder]->Play(false);

				break;
			}
		}

		int pre = PLAY_SOUND_MAX - PLAY_SOUND_MIN;
		thisTimePlaySound = rand() % pre + PLAY_SOUND_MIN;
		timer = 0;
	}
}

#pragma endregion


#pragma region Day3
void SceneGame3::Init()
{
	// エネミーの生成間隔
	GENERATE_ENEMY_MIN = 300;
	GENERATE_ENEMY_MAX = 500;
	// 敵ごとの生成確率
	GENERATE_PER_ENEMY1 = 70;
	GENERATE_PER_ENEMY2 = 30;
	GENERATE_PER_ENEMY3 = 0;
	generateEnemyCount = 0;
	thisTimeGeneratePerCount = 90;

	nightTime = 0;

	nowDay = 2;


	// デバッグ用
	debugLightPos[0] = DirectX::XMFLOAT3(-38.0f, 25.0f, 75.0f);
	debugLightPos[1] = DirectX::XMFLOAT3(38.0f, 25.0f, 75.0f);
	debugLightPos[2] = DirectX::XMFLOAT3(0.0f, 60.0f, 2.0f);
	debugLightPos[3] = DirectX::XMFLOAT3(-100.0f, 10.0f, 85.0f);
	debugLightPos[4] = DirectX::XMFLOAT3(100.0f, 10.0f, 85.0f);


	debugLightRange[0] = 100.0f;
	debugLightRange[1] = 100.0f;
	debugLightRange[2] = 160.0f;
	debugLightRange[3] = 0.0f;
	debugLightRange[4] = 0.0f;


	// コンスタントバッファ作成
	CreateConstantBuffer(&constantBuffer, sizeof(LightConstantBuffer));

	Stage::Get().Init();
	EnemyManager::Get().Init();
	Sound::Get().Init();

	// ライトの設定
	Light::Init();
	Light::SetAmbient(DirectX::XMFLOAT3(0.1f, 0.1f, 0.1f));

	lightDir.x = sinf(DirectX::XM_PI);
	lightDir.y = -0.4f;
	lightDir.z = cosf(DirectX::XM_PI);
	Light::SetDirLight(lightDir, DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f));

	Light::SetPointLight(0, debugLightPos[0]/*DirectX::XMFLOAT3(-50.0f, -7.0f, 27.0f)*/, DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f), debugLightRange[0]);	// 左
	Light::SetPointLight(1, debugLightPos[1]/*DirectX::XMFLOAT3(50.0f, -7.0f, 27.0f)*/, DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f), debugLightRange[1]);	// 右
	Light::SetPointLight(2, debugLightPos[2]/*DirectX::XMFLOAT3(0.0f, -26.0f, 63.0f)*/, DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f), debugLightRange[2]);	// 真ん中
	Light::TurnOffPointLight(0);
	Light::TurnOffPointLight(1);
	Light::amountState = Light::ConsumptionAmount::Small;

	sound[SoundType::Chaim] = new Audio("Data/Sound/チャイム.wav");
	sound[SoundType::OpenDoor] = new Audio("Data/Sound/ドアの音.wav");
	sound[SoundType::Rain] = new Audio("Data/Sound/雨.wav");
	sound[SoundType::Pass] = new Audio("Data/Sound/通りゃんせ.wav");
	sound[SoundType::RingTone] = new Audio("Data/Sound/電話.wav");
	sound[SoundType::Wind] = new Audio("Data/Sound/風で木の葉が揺れる音.wav");
	sound[SoundType::Walk] = new Audio("Data/Sound/歩く音ブーツ.wav");
	sound[SoundType::Thunder] = new Audio("Data/Sound/雷.wav");

	PLAY_SOUND_MAX = 300;
	PLAY_SOUND_MIN = 180;
	SOUND_VOLUME_MAX = 1.0f;
	SOUND_VOLUME_MIN = 0.5f;
	int tmp = PLAY_SOUND_MAX - PLAY_SOUND_MIN;
	thisTimePlaySound = rand() % tmp + PLAY_SOUND_MIN;
	tmp = SOUND_VOLUME_MAX - SOUND_VOLUME_MIN;
	SoundVol = rand() % tmp + SOUND_VOLUME_MIN;

	for (auto& it : soundVol)it = 1.0f;
}

void SceneGame3::Update()
{
#ifdef USE_IMGUI
	UseImGui();
#endif
	// ポイントライトの設定
	Light::SetPointLight(2, debugLightPos[2]/*DirectX::XMFLOAT3(0.0f, -26.0f, 63.0f)*/, DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f), debugLightRange[2]);	// 真ん中
//	Light::SetPointLight(3, debugLightPos[3]/*DirectX::XMFLOAT3(0.0f, -26.0f, 63.0f)*/,		DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f), debugLightRange[3]);	// 左中
//	Light::SetPointLight(4, debugLightPos[4]/*DirectX::XMFLOAT3(0.0f, -26.0f, 63.0f)*/,		DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f), debugLightRange[4]);	// 右中

	Light::Update();
	Camera::Get().Update();
	Stage::Get().Update();
	EnemyManager::Get().Update();
	Sound::Get().Update();

	GenerateEnemy();
	JudgeGoClear();
	PlayEnvironment();

	nightTime++;
}

void SceneGame3::Render()
{
	using namespace DirectX;

	// 行列設定
	XMMATRIX V, P;
	V = Camera::Get().GetViewMatrix();
	P = Camera::Get().GetProjectionMatrix();

	// 定数バッファの設定
	SetConstantBuffer();

	Stage::Get().Render(V, P);
	EnemyManager::Get().Render(V, P);
}

void SceneGame3::Uninit()
{

}

#ifdef USE_IMGUI
void SceneGame3::UseImGui()
{
	ImGui::Begin("SceneGame Day3");

	if (ImGui::TreeNode("PointLight Status"))
	{
		if (ImGui::TreeNode(" No.1 "))
		{
			ImGui::SliderFloat3("Position ", &debugLightPos[0].x, -100.0f, 100.0f);
			ImGui::SliderFloat("Range ", &debugLightRange[0], 0.1f, 1000.0f);
			ImGui::TreePop();
		}
		if (ImGui::TreeNode(" No.2 "))
		{
			ImGui::SliderFloat3("Position ", &debugLightPos[1].x, -100.0f, 100.0f);
			ImGui::SliderFloat("Range ", &debugLightRange[1], 0.1f, 1000.0f);
			ImGui::TreePop();
		}
		if (ImGui::TreeNode(" No.3 "))
		{
			ImGui::SliderFloat3("Position ", &debugLightPos[2].x, -100.0f, 100.0f);
			ImGui::SliderFloat("Range ", &debugLightRange[2], 0.1f, 1000.0f);
			ImGui::TreePop();
		}
		if (ImGui::TreeNode(" No.4 "))
		{
			ImGui::SliderFloat3("Position ", &debugLightPos[3].x, -100.0f, 100.0f);
			ImGui::SliderFloat("Range ", &debugLightRange[3], 0.1f, 1000.0f);
			ImGui::TreePop();
		}
		if (ImGui::TreeNode(" No.5 "))
		{
			ImGui::SliderFloat3("Position ", &debugLightPos[4].x, -200.0f, 200.0f);
			ImGui::SliderFloat("Range ", &debugLightRange[4], 0.1f, 1000.0f);
			ImGui::TreePop();
		}


		ImGui::TreePop();
	}

	if (ImGui::TreeNode("Generate Enemy Parameter"))
	{
		ImGui::InputInt("GENERATE_PER_ENEMY1", &GENERATE_PER_ENEMY1);
		ImGui::InputInt("GENERATE_PER_ENEMY2", &GENERATE_PER_ENEMY2);
		ImGui::InputInt("GENERATE_PER_ENEMY3", &GENERATE_PER_ENEMY3);

		ImGui::InputInt("GENERATE_ENEMY_MAX", &GENERATE_ENEMY_MAX);
		ImGui::InputInt("GENERATE_ENEMY_MIN", &GENERATE_ENEMY_MIN);
		ImGui::TreePop();
	}

	ImGui::Text(" nightTime : %d", nightTime);
	ImGui::InputInt("END_OF_NIGHT", &TOTAL_NIGHTS);
	ImGui::End();

}
#endif

void SceneGame3::GenerateEnemy()
{
	generateEnemyCount++;

	if (generateEnemyCount >= thisTimeGeneratePerCount)
	{
		int generateEnemy1Per = 100 - GENERATE_PER_ENEMY1;
		int generateEnemy2Per = 100 - GENERATE_PER_ENEMY2;
		int generateEnemy3Per = 100 - GENERATE_PER_ENEMY3;

		if (rand() % 200 >= 100)
		{
			if (rand() % 100 >= generateEnemy1Per)
			{
				EnemyManager::Get().GenerateEnemy(Enemy::EnemyType::Daruma, Enemy::Point::R_Point5);
			}
			else if (rand() % 100 >= generateEnemy2Per)
			{
				EnemyManager::Get().GenerateEnemy(Enemy::EnemyType::Ghost, Enemy::Point::R_Point5);
			}
			else if (rand() % 100 >= generateEnemy3Per)
			{
				EnemyManager::Get().GenerateEnemy(Enemy::EnemyType::TotemPole, Enemy::Point::R_Point5);
			}
		}
		else
		{
			if (rand() % 100 >= generateEnemy1Per)
			{
				EnemyManager::Get().GenerateEnemy(Enemy::EnemyType::Daruma, Enemy::Point::L_Point5);
			}
			else if (rand() % 100 >= generateEnemy2Per)
			{
				EnemyManager::Get().GenerateEnemy(Enemy::EnemyType::Ghost, Enemy::Point::L_Point5);
			}
			else if (rand() % 100 >= generateEnemy3Per)
			{
				EnemyManager::Get().GenerateEnemy(Enemy::EnemyType::TotemPole, Enemy::Point::L_Point5);
			}

		}
		int tmp = GENERATE_ENEMY_MAX - GENERATE_ENEMY_MIN;
		thisTimeGeneratePerCount = rand() % tmp + GENERATE_ENEMY_MIN;
		generateEnemyCount = 0;
	}
}

void SceneGame3::JudgeGoClear()
{
	if (nightTime >= TOTAL_NIGHTS)
	{
		SceneManager::Get().SetScene(SceneManager::SCENE::GAME_CLEAR);
	}
}

void SceneGame3::PlayEnvironment()
{
	static int timer;
	if (++timer >= thisTimePlaySound)
	{
		float tmp = SOUND_VOLUME_MAX - SOUND_VOLUME_MIN;
		int tmp_i = 100 * tmp;
		float tmpVol = static_cast<float>(rand() % tmp_i + SOUND_VOLUME_MIN * 100) / 100;
		if (rand() % 200 >= 100)
		{
			// 右
			int rnd = rand() % SoundType::Max;
			switch (rnd)
			{
			case 0:		// Chaim
				sound[SoundType::Chaim]->SetPosition(DirectX::XMFLOAT3(38.0f, 0.0f, 0.0f));
				sound[SoundType::Chaim]->SetVolume(tmpVol);
				sound[SoundType::Chaim]->Play(false);
				break;
			case 1:		// OpenDoor
				sound[SoundType::OpenDoor]->SetPosition(DirectX::XMFLOAT3(38.0f, 0.0f, 0.0f));
				sound[SoundType::OpenDoor]->SetVolume(tmpVol);
				sound[SoundType::OpenDoor]->Play(false);
				break;
			case 2:		// Rain
				sound[SoundType::Rain]->SetPosition(DirectX::XMFLOAT3(38.0f, 0.0f, 0.0f));
				sound[SoundType::Rain]->SetVolume(tmpVol);
				sound[SoundType::Rain]->Play(false);
				break;
			case 3:		// Pass
				sound[SoundType::Pass]->SetPosition(DirectX::XMFLOAT3(38.0f, 0.0f, 0.0f));
				sound[SoundType::Pass]->Play(false);
				break;
			case 4:		// RingTone
				sound[SoundType::RingTone]->SetPosition(DirectX::XMFLOAT3(38.0f, 0.0f, 0.0f));
				sound[SoundType::RingTone]->SetVolume(tmpVol);
				sound[SoundType::RingTone]->Play(false);
				break;
			case 5:		// Wind
				sound[SoundType::Wind]->SetPosition(DirectX::XMFLOAT3(38.0f, 0.0f, 0.0f));
				sound[SoundType::Wind]->SetVolume(tmpVol);
				sound[SoundType::Wind]->Play(false);
				break;
			case 6:		// Walk
				sound[SoundType::Walk]->SetPosition(DirectX::XMFLOAT3(38.0f, 0.0f, 0.0f));
				sound[SoundType::Walk]->SetVolume(tmpVol);
				sound[SoundType::Walk]->Play(false);
				break;
			case 7:		// Thunder
				sound[SoundType::Thunder]->SetPosition(DirectX::XMFLOAT3(38.0f, 0.0f, 0.0f));
				sound[SoundType::Thunder]->SetVolume(tmpVol);
				sound[SoundType::Thunder]->Play(false);

				break;
			}
		}
		else
		{
			// 左
			int rnd = rand() % SoundType::Max;
			switch (rnd)
			{
			case 0:		// Chaim
				sound[SoundType::Chaim]->SetPosition(DirectX::XMFLOAT3(-38.0f, 0.0f, 0.0f));
				sound[SoundType::Chaim]->SetVolume(tmpVol);
				sound[SoundType::Chaim]->Play(false);
				break;
			case 1:		// OpenDoor
				sound[SoundType::OpenDoor]->SetPosition(DirectX::XMFLOAT3(-38.0f, 0.0f, 0.0f));
				sound[SoundType::OpenDoor]->SetVolume(tmpVol);
				sound[SoundType::OpenDoor]->Play(false);
				break;
			case 2:		// Rain
				sound[SoundType::Rain]->SetPosition(DirectX::XMFLOAT3(-38.0f, 0.0f, 0.0f));
				sound[SoundType::Rain]->SetVolume(tmpVol);
				sound[SoundType::Rain]->Play(false);
				break;
			case 3:		// Pass
				sound[SoundType::Pass]->SetPosition(DirectX::XMFLOAT3(-38.0f, 0.0f, 0.0f));
				sound[SoundType::Pass]->Play(false);
				break;
			case 4:		// RingTone
				sound[SoundType::RingTone]->SetPosition(DirectX::XMFLOAT3(-38.0f, 0.0f, 0.0f));
				sound[SoundType::RingTone]->SetVolume(tmpVol);
				sound[SoundType::RingTone]->Play(false);
				break;
			case 5:		// Wind
				sound[SoundType::Wind]->SetPosition(DirectX::XMFLOAT3(-38.0f, 0.0f, 0.0f));
				sound[SoundType::Wind]->SetVolume(tmpVol);
				sound[SoundType::Wind]->Play(false);
				break;
			case 6:		// Walk
				sound[SoundType::Walk]->SetPosition(DirectX::XMFLOAT3(-38.0f, 0.0f, 0.0f));
				sound[SoundType::Walk]->SetVolume(tmpVol);
				sound[SoundType::Walk]->Play(false);
				break;
			case 7:		// Thunder
				sound[SoundType::Thunder]->SetPosition(DirectX::XMFLOAT3(-38.0f, 0.0f, 0.0f));
				sound[SoundType::Thunder]->SetVolume(tmpVol);
				sound[SoundType::Thunder]->Play(false);

				break;
			}
		}

		int pre = PLAY_SOUND_MAX - PLAY_SOUND_MIN;
		thisTimePlaySound = rand() % pre + PLAY_SOUND_MIN;
		timer = 0;
	}
}


#pragma endregion