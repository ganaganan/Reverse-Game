#include "SceneGame.h"

#include <DirectXMath.h>
#include <framework.h>

#include "Camera.h"
#include "Stage.h"
#include "Enemy.h"
#include "Light.h"
#include "Sound.h"
#include "../MyLib/GamePad.h"
#include "UI.h"

#pragma region Day1
int SceneGame::nightTime;
void SceneGame::Init()
{
	// �G�l�~�[�̐����Ԋu
	GENERATE_ENEMY_MIN = 1620;
	GENERATE_ENEMY_MAX = 1800;
	// �G���Ƃ̐����m��
	GENERATE_PER_ENEMY1 = 70;
	GENERATE_PER_ENEMY2 = 30;
	GENERATE_PER_ENEMY3 = 0;
	generateEnemyCount = 0;
	thisTimeGeneratePerCount = 90;

	nightTime = 0;

	nowDay = 0;

	// �f�o�b�O�p
	debugLightPos[0] = DirectX::XMFLOAT3(-38.0f, 25.0f, 55.0f);
	debugLightPos[1] = DirectX::XMFLOAT3(38.0f, 25.0f, 55.0f);
	debugLightPos[2] = DirectX::XMFLOAT3(0.0f, 60.0f, 2.0f);
	debugLightPos[3] = DirectX::XMFLOAT3(-38.0f, 25.0f, 55.0f);
	debugLightPos[4] = DirectX::XMFLOAT3(38.0f, 25.0f, 55.0f);


	debugLightRange[0] = 50.0f;
	debugLightRange[1] = 50.0f;
	debugLightRange[2] = 110.0f;
	debugLightRange[3] = 110.0f;
	debugLightRange[4] = 110.0f;

	// �R���X�^���g�o�b�t�@�쐬
	CreateConstantBuffer(&constantBuffer, sizeof(LightConstantBuffer));

	Stage::Get().Init();
	EnemyManager::Get().Init();
	Sound::Get().Init();
	Camera::Get().Init();
	UI::Get().Init();

	// ���C�g�̐ݒ�
	Light::Init();
	Light::SetAmbient(DirectX::XMFLOAT3(0.1f, 0.1f, 0.1f));

	lightDir.x = sinf(DirectX::XM_PI);
	lightDir.y = -0.4f;
	lightDir.z = cosf(DirectX::XM_PI);
	Light::SetDirLight(lightDir, DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f));

	Light::SetPointLight(0, debugLightPos[0]/*DirectX::XMFLOAT3(-50.0f, -7.0f, 27.0f)*/, DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f), debugLightRange[0]);	// ��
	Light::SetPointLight(1, debugLightPos[1]/*DirectX::XMFLOAT3(50.0f, -7.0f, 27.0f)*/, DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f), debugLightRange[1]);	// �E
	Light::SetPointLight(2, debugLightPos[2]/*DirectX::XMFLOAT3(0.0f, -26.0f, 63.0f)*/, DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f), debugLightRange[2]);	// �^��
	Light::SetPointLight(3, debugLightPos[3]/*DirectX::XMFLOAT3(-50.0f, -7.0f, 27.0f)*/, DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f), debugLightRange[3]);	// �� �蓮
	Light::SetPointLight(4, debugLightPos[4]/*DirectX::XMFLOAT3(-50.0f, -7.0f, 27.0f)*/, DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f), debugLightRange[4]);	// �E �蓮
	Light::TurnOffPointLight(3);
	Light::TurnOffPointLight(4);
	Light::amountState = Light::ConsumptionAmount::Small;


	// Set Sound
	sound[SoundType::Chaim] = new Audio("Data/Sound/�`���C��.wav");
	sound[SoundType::OpenDoor] = new Audio("Data/Sound/�h�A�̉�.wav");
	sound[SoundType::Rain] = new Audio("Data/Sound/�J.wav");
	sound[SoundType::Pass] = new Audio("Data/Sound/�ʂ���.wav");
	sound[SoundType::RingTone] = new Audio("Data/Sound/�d�b.wav");
	sound[SoundType::Wind] = new Audio("Data/Sound/���Ŗ؂̗t���h��鉹.wav");
	sound[SoundType::Walk] = new Audio("Data/Sound/�������u�[�c.wav");
	sound[SoundType::Thunder] = new Audio("Data/Sound/��.wav");

	PLAY_SOUND_MAX = 1200;
	PLAY_SOUND_MIN = 900;
	SOUND_VOLUME_MAX = 1.0f;
	SOUND_VOLUME_MIN = 0.5f;
	int tmp = PLAY_SOUND_MAX - PLAY_SOUND_MIN;
	thisTimePlaySound = rand() % tmp + PLAY_SOUND_MIN;
	tmp = SOUND_VOLUME_MAX - SOUND_VOLUME_MIN;
	SoundVol = rand() % tmp + SOUND_VOLUME_MIN;

	for (auto& it : soundVol)it = 1.0f;

	goDay2 = false;
	canSeeString = false;
	fadeTime = 0;
	fadePosYUp = -framework::SCREEN_HEIGHT / 2 - 50.0f;
	fadePosYDown = framework::SCREEN_HEIGHT;
	count = 0;
}

void SceneGame::Update()
{	
//	if (InputTrigger(XINPUT_Y))
//	{
//		SceneManager::Get().SetScene(SceneManager::GAME_CLEAR);
//	}
#ifdef USE_IMGUI
	UseImGui();
#endif
	// �|�C���g���C�g�̐ݒ�
//	Light::SetPointLight(0, debugLightPos[0]/*DirectX::XMFLOAT3(0.0f, -26.0f, 63.0f)*/, DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f), debugLightRange[0]);	// �^��
//	Light::SetPointLight(1, debugLightPos[1]/*DirectX::XMFLOAT3(0.0f, -26.0f, 63.0f)*/, DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f), debugLightRange[1]);	// �^��
//	Light::SetPointLight(2, debugLightPos[2]/*DirectX::XMFLOAT3(0.0f, -26.0f, 63.0f)*/,		DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f), debugLightRange[2]);	// �^��
//	Light::SetPointLight(3, debugLightPos[3]/*DirectX::XMFLOAT3(0.0f, -26.0f, 63.0f)*/,		DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f), debugLightRange[3]);	// ����
//	Light::SetPointLight(4, debugLightPos[4]/*DirectX::XMFLOAT3(0.0f, -26.0f, 63.0f)*/,		DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f), debugLightRange[4]);	// �E��

	Light::Update();
	Camera::Get().Update();
	Stage::Get().Update();
	EnemyManager::Get().Update();
	Sound::Get().Update();
	UI::Get().Update();

	GenerateEnemy();

	JudgeGoNextDay();

//	PlayEnvironment();

	nightTime++;
}

void SceneGame::Render()
{
	using namespace DirectX;

	// �s��ݒ�
	XMMATRIX V, P;
	V =	Camera::Get().GetViewMatrix();
	P =	Camera::Get().GetProjectionMatrix();

	// �萔�o�b�t�@�̐ݒ�
	SetConstantBuffer();

	Stage::Get().Render(V, P);
	EnemyManager::Get().Render(V, P);
	UI::Get().Render();

	SetDrawBlendMode(BLEND_MODE::ALPHA);
	UI::Get().sprBlack->Begin();
	UI::Get().sprBlack->Draw(DirectX::XMFLOAT2(0.0f, fadePosYUp), DirectX::XMFLOAT2(framework::SCREEN_WIDTH + 100.0f, framework::SCREEN_HEIGHT / 2 ),
		DirectX::XMFLOAT2(0, 0), DirectX::XMFLOAT2(1980, 1080), DirectX::XMFLOAT2(0.0f, 0.0f), 0.0f, DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
	UI::Get().sprBlack->Draw(DirectX::XMFLOAT2(0.0f, fadePosYDown), DirectX::XMFLOAT2(framework::SCREEN_WIDTH + 100.0f, framework::SCREEN_HEIGHT / 2 + 100),
		DirectX::XMFLOAT2(0, 0), DirectX::XMFLOAT2(1980, 1080), DirectX::XMFLOAT2(0.0f, 0.0f), 0.0f, DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
	UI::Get().sprBlack->End();
	if (canSeeString)
	{
		UI::Get().sprGameOver->Begin();
		UI::Get().sprGameOver->Draw(700, 450, 475, 128, 512, 256, 475, 128, 0, 0, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f);
		UI::Get().sprGameOver->End();
	}
	SetDrawBlendMode(BLEND_MODE::NONE);
}

void SceneGame::Uninit()
{
	for (auto& it : sound)
	{
		delete it;
		it = nullptr;
	}
	UI::Get().Uninit();
	EnemyManager::Get().Uninit();
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
		goDay2 = true;
	}
	if (goDay2)
	{
		fadeTime++;
		if (fadeTime <= 30)
		{
			fadePosYUp += (framework::SCREEN_HEIGHT / 2 + 50.0f) * 1 / 30;
			fadePosYDown -= (framework::SCREEN_HEIGHT / 2 + 50.0f) * 1 / 30;
		}
		else if (count++ < 60)
		{
			canSeeString = true;
		}
		else
		{
			SceneManager::Get().SetScene(SceneManager::MAIN_GAME2);
		}
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
			// �E
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
			// ��
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
int SceneGame2::nightTime;
void SceneGame2::Init()
{
	// �G�l�~�[�̐����Ԋu
	GENERATE_ENEMY_MIN = 840;
	GENERATE_ENEMY_MAX = 900;
	// �G���Ƃ̐����m��
	GENERATE_PER_ENEMY1 = 40;
	GENERATE_PER_ENEMY2 = 40;
	GENERATE_PER_ENEMY3 = 20;
	generateEnemyCount = 0;
	thisTimeGeneratePerCount = 90;

	nightTime = 0;

	nowDay = 1;

	// �f�o�b�O�p
	debugLightPos[0] = DirectX::XMFLOAT3(-38.0f, 25.0f, 55.0f);
	debugLightPos[1] = DirectX::XMFLOAT3(38.0f, 25.0f, 55.0f);
	debugLightPos[2] = DirectX::XMFLOAT3(0.0f, 60.0f, 2.0f);
	debugLightPos[3] = DirectX::XMFLOAT3(-38.0f, 25.0f, 55.0f);
	debugLightPos[4] = DirectX::XMFLOAT3(38.0f, 25.0f, 55.0f);


	debugLightRange[0] = 50.0f;
	debugLightRange[1] = 50.0f;
	debugLightRange[2] = 110.0f;
	debugLightRange[3] = 110.0f;
	debugLightRange[4] = 110.0f;


	// �R���X�^���g�o�b�t�@�쐬
	CreateConstantBuffer(&constantBuffer, sizeof(LightConstantBuffer));

	Stage::Get().Init();
	EnemyManager::Get().Init();
	Sound::Get().Init();
	Camera::Get().Init();
	UI::Get().Init();

	// ���C�g�̐ݒ�
	Light::Init();
	Light::SetAmbient(DirectX::XMFLOAT3(0.1f, 0.1f, 0.1f));

	lightDir.x = sinf(DirectX::XM_PI);
	lightDir.y = -0.4f;
	lightDir.z = cosf(DirectX::XM_PI);
	Light::SetDirLight(lightDir, DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f));

	Light::SetPointLight(0, debugLightPos[0]/*DirectX::XMFLOAT3(-50.0f, -7.0f, 27.0f)*/, DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f), debugLightRange[0]);	// ��
	Light::SetPointLight(1, debugLightPos[1]/*DirectX::XMFLOAT3(50.0f, -7.0f, 27.0f)*/, DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f), debugLightRange[1]);	// �E
	Light::SetPointLight(2, debugLightPos[2]/*DirectX::XMFLOAT3(0.0f, -26.0f, 63.0f)*/, DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f), debugLightRange[2]);	// �^��
	Light::SetPointLight(3, debugLightPos[3]/*DirectX::XMFLOAT3(-50.0f, -7.0f, 27.0f)*/, DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f), debugLightRange[3]);	// �� �蓮
	Light::SetPointLight(4, debugLightPos[4]/*DirectX::XMFLOAT3(-50.0f, -7.0f, 27.0f)*/, DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f), debugLightRange[4]);	// �E �蓮
	Light::TurnOffPointLight(3);
	Light::TurnOffPointLight(4);
	Light::amountState = Light::ConsumptionAmount::Small;

	sound[SoundType::Chaim] = new Audio("Data/Sound/�`���C��.wav");
	sound[SoundType::OpenDoor] = new Audio("Data/Sound/�h�A�̉�.wav");
	sound[SoundType::Rain] = new Audio("Data/Sound/�J.wav");
	sound[SoundType::Pass] = new Audio("Data/Sound/�ʂ���.wav");
	sound[SoundType::RingTone] = new Audio("Data/Sound/�d�b.wav");
	sound[SoundType::Wind] = new Audio("Data/Sound/���Ŗ؂̗t���h��鉹.wav");
	sound[SoundType::Walk] = new Audio("Data/Sound/�������u�[�c.wav");
	sound[SoundType::Thunder] = new Audio("Data/Sound/��.wav");

	PLAY_SOUND_MAX = 1200;
	PLAY_SOUND_MIN = 900;
	SOUND_VOLUME_MAX = 1.0f;
	SOUND_VOLUME_MIN = 0.5f;
	int tmp = PLAY_SOUND_MAX - PLAY_SOUND_MIN;
	thisTimePlaySound = rand() % tmp + PLAY_SOUND_MIN;
	tmp = SOUND_VOLUME_MAX - SOUND_VOLUME_MIN;
	SoundVol = rand() % tmp + SOUND_VOLUME_MIN;

	for (auto& it : soundVol)it = 1.0f;

	canSeeString = false;
	byDay1 = true;
	goDay3 = false;
	fadeTime = 0;
	fadePosYUp = 0.0f;
	fadePosYDown = framework::SCREEN_HEIGHT / 2;
	count = 0;

}

void SceneGame2::Update()
{
	if (byDay1)
	{
		fadeTime++;
		if (fadeTime <= 24)
		{
			fadePosYUp -= (framework::SCREEN_HEIGHT / 2) * 1 / 24;
			fadePosYDown += (framework::SCREEN_HEIGHT / 2) * 1 / 24;
		}
		else
		{
			fadeTime = 0;
			byDay1 = false;
		}
	}

#ifdef USE_IMGUI
	UseImGui();
#endif
	// �|�C���g���C�g�̐ݒ�
//	Light::SetPointLight(2, debugLightPos[2], DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f), debugLightRange[2]);	// �^��

	Light::Update();
	Camera::Get().Update();
	Stage::Get().Update();
	EnemyManager::Get().Update();
	Sound::Get().Update();
	UI::Get().Update();

	GenerateEnemy();
	JudgeGoNextDay();
//	PlayEnvironment();

	nightTime++;
}

void SceneGame2::Render()
{
	using namespace DirectX;

	// �s��ݒ�
	XMMATRIX V, P;
	V = Camera::Get().GetViewMatrix();
	P = Camera::Get().GetProjectionMatrix();

	// �萔�o�b�t�@�̐ݒ�
	SetConstantBuffer();

	Stage::Get().Render(V, P);
	EnemyManager::Get().Render(V, P);
	UI::Get().Render();

	SetDrawBlendMode(BLEND_MODE::ALPHA);
	UI::Get().sprBlack->Begin();
	UI::Get().sprBlack->Draw(DirectX::XMFLOAT2(0.0f, fadePosYUp), DirectX::XMFLOAT2(framework::SCREEN_WIDTH + 100.0f, framework::SCREEN_HEIGHT / 2),
		DirectX::XMFLOAT2(0, 0), DirectX::XMFLOAT2(1980, 1080), DirectX::XMFLOAT2(0.0f, 0.0f), 0.0f, DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
	UI::Get().sprBlack->Draw(DirectX::XMFLOAT2(0.0f, fadePosYDown), DirectX::XMFLOAT2(framework::SCREEN_WIDTH + 100.0f, framework::SCREEN_HEIGHT / 2 + 100),
		DirectX::XMFLOAT2(0, 0), DirectX::XMFLOAT2(1980, 1080), DirectX::XMFLOAT2(0.0f, 0.0f), 0.0f, DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
	UI::Get().sprBlack->End();
	if (canSeeString)
	{
		UI::Get().sprGameOver->Begin();
		UI::Get().sprGameOver->Draw(700, 450, 473, 128, 0, 384, 473, 128, 0, 0, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f);
		UI::Get().sprGameOver->End();
	}
	SetDrawBlendMode(BLEND_MODE::NONE);

}

void SceneGame2::Uninit()
{
	for (auto& it : sound)
	{
		delete it;
		it = nullptr;
	}
	EnemyManager::Get().Uninit();
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
//	if (nightTime >= TOTAL_NIGHTS)
//	{
//		SceneManager::Get().SetScene(SceneManager::MAIN_GAME3);
//	}

	if (nightTime >= TOTAL_NIGHTS)
	{
		goDay3 = true;
	}
	if (goDay3)
	{
		fadeTime++;
		if (fadeTime <= 24)
		{
			fadePosYUp += (framework::SCREEN_HEIGHT / 2) * 1 / 24;
			fadePosYDown -= (framework::SCREEN_HEIGHT / 2) * 1 / 24;
		}
		else if (count++ < 60)
		{
			canSeeString = true;
		}
		else
		{
			SceneManager::Get().SetScene(SceneManager::MAIN_GAME3);
		}
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
			// �E
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
			// ��
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
int SceneGame3::nightTime;
void SceneGame3::Init()
{
	// �G�l�~�[�̐����Ԋu
	GENERATE_ENEMY_MIN = 500;
	GENERATE_ENEMY_MAX = 540;
	// �G���Ƃ̐����m��
	GENERATE_PER_ENEMY1 = 30;
	GENERATE_PER_ENEMY2 = 40;
	GENERATE_PER_ENEMY3 = 30;
	generateEnemyCount = 0;
	thisTimeGeneratePerCount = 90;

	nightTime = 0;

	nowDay = 2;


	// �f�o�b�O�p
	debugLightPos[0] = DirectX::XMFLOAT3(-38.0f, 25.0f, 55.0f);
	debugLightPos[1] = DirectX::XMFLOAT3(38.0f, 25.0f, 55.0f);
	debugLightPos[2] = DirectX::XMFLOAT3(0.0f, 60.0f, 2.0f);
	debugLightPos[3] = DirectX::XMFLOAT3(-38.0f, 25.0f, 55.0f);
	debugLightPos[4] = DirectX::XMFLOAT3(38.0f, 25.0f, 55.0f);


	debugLightRange[0] = 50.0f;
	debugLightRange[1] = 50.0f;
	debugLightRange[2] = 105.0f;
	debugLightRange[3] = 110.0f;
	debugLightRange[4] = 110.0f;


	// �R���X�^���g�o�b�t�@�쐬
	CreateConstantBuffer(&constantBuffer, sizeof(LightConstantBuffer));

	Stage::Get().Init();
	EnemyManager::Get().Init();
	Sound::Get().Init();	
	Camera::Get().Init();
	UI::Get().Init();

	// ���C�g�̐ݒ�
	Light::Init();
	Light::SetAmbient(DirectX::XMFLOAT3(0.1f, 0.1f, 0.1f));

	lightDir.x = sinf(DirectX::XM_PI);
	lightDir.y = -0.4f;
	lightDir.z = cosf(DirectX::XM_PI);
	Light::SetDirLight(lightDir, DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f));

	Light::SetPointLight(0, debugLightPos[0]/*DirectX::XMFLOAT3(-50.0f, -7.0f, 27.0f)*/, DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f), debugLightRange[0]);	// ��
	Light::SetPointLight(1, debugLightPos[1]/*DirectX::XMFLOAT3(50.0f, -7.0f, 27.0f)*/, DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f), debugLightRange[1]);	// �E
	Light::SetPointLight(2, debugLightPos[2]/*DirectX::XMFLOAT3(0.0f, -26.0f, 63.0f)*/, DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f), debugLightRange[2]);	// �^��
	Light::SetPointLight(3, debugLightPos[3]/*DirectX::XMFLOAT3(-50.0f, -7.0f, 27.0f)*/, DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f), debugLightRange[3]);	// �� �蓮
	Light::SetPointLight(4, debugLightPos[4]/*DirectX::XMFLOAT3(-50.0f, -7.0f, 27.0f)*/, DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f), debugLightRange[4]);	// �E �蓮
	Light::TurnOffPointLight(3);
	Light::TurnOffPointLight(4);
	Light::amountState = Light::ConsumptionAmount::Small;

	sound[SoundType::Chaim] = new Audio("Data/Sound/�`���C��.wav");
	sound[SoundType::OpenDoor] = new Audio("Data/Sound/�h�A�̉�.wav");
	sound[SoundType::Rain] = new Audio("Data/Sound/�J.wav");
	sound[SoundType::Pass] = new Audio("Data/Sound/�ʂ���.wav");
	sound[SoundType::RingTone] = new Audio("Data/Sound/�d�b.wav");
	sound[SoundType::Wind] = new Audio("Data/Sound/���Ŗ؂̗t���h��鉹.wav");
	sound[SoundType::Walk] = new Audio("Data/Sound/�������u�[�c.wav");
	sound[SoundType::Thunder] = new Audio("Data/Sound/��.wav");

	PLAY_SOUND_MAX = 1200;
	PLAY_SOUND_MIN = 900;
	SOUND_VOLUME_MAX = 1.0f;
	SOUND_VOLUME_MIN = 0.5f;
	int tmp = PLAY_SOUND_MAX - PLAY_SOUND_MIN;
	thisTimePlaySound = rand() % tmp + PLAY_SOUND_MIN;
	tmp = SOUND_VOLUME_MAX - SOUND_VOLUME_MIN;
	SoundVol = rand() % tmp + SOUND_VOLUME_MIN;

	for (auto& it : soundVol)it = 1.0f;

	byDay2 = true;
	fadeTime = 0;
	fadePosYUp = 0.0f;
	fadePosYDown = framework::SCREEN_HEIGHT / 2;
//	count = 0;
}

void SceneGame3::Update()
{
	if (byDay2)
	{
		fadeTime++;
		if (fadeTime <= 24)
		{
			fadePosYUp -= (framework::SCREEN_HEIGHT / 2) * 1 / 24;
			fadePosYDown += (framework::SCREEN_HEIGHT / 2) * 1 / 24;
		}
		else
		{
			fadeTime = 0;
			byDay2 = false;
		}
	}
#ifdef USE_IMGUI
	UseImGui();
#endif
	// �|�C���g���C�g�̐ݒ�
//	Light::SetPointLight(2, debugLightPos[2]/*DirectX::XMFLOAT3(0.0f, -26.0f, 63.0f)*/, DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f), debugLightRange[2]);	// �^��
//	Light::SetPointLight(3, debugLightPos[3]/*DirectX::XMFLOAT3(0.0f, -26.0f, 63.0f)*/,		DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f), debugLightRange[3]);	// ����
//	Light::SetPointLight(4, debugLightPos[4]/*DirectX::XMFLOAT3(0.0f, -26.0f, 63.0f)*/,		DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f), debugLightRange[4]);	// �E��

	Light::Update();
	Camera::Get().Update();
	Stage::Get().Update();
	EnemyManager::Get().Update();
	Sound::Get().Update();
	UI::Get().Update();

	GenerateEnemy();
	JudgeGoClear();
//	PlayEnvironment();

	nightTime++;
}

void SceneGame3::Render()
{
	using namespace DirectX;

	// �s��ݒ�
	XMMATRIX V, P;
	V = Camera::Get().GetViewMatrix();
	P = Camera::Get().GetProjectionMatrix();

	// �萔�o�b�t�@�̐ݒ�
	SetConstantBuffer();

	Stage::Get().Render(V, P);
	EnemyManager::Get().Render(V, P);
	UI::Get().Render();


	SetDrawBlendMode(BLEND_MODE::ALPHA);
	UI::Get().sprBlack->Begin();
	UI::Get().sprBlack->Draw(DirectX::XMFLOAT2(0.0f, fadePosYUp), DirectX::XMFLOAT2(framework::SCREEN_WIDTH + 100.0f, framework::SCREEN_HEIGHT / 2),
		DirectX::XMFLOAT2(0, 0), DirectX::XMFLOAT2(1980, 1080), DirectX::XMFLOAT2(0.0f, 0.0f), 0.0f, DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
	UI::Get().sprBlack->Draw(DirectX::XMFLOAT2(0.0f, fadePosYDown), DirectX::XMFLOAT2(framework::SCREEN_WIDTH + 100.0f, framework::SCREEN_HEIGHT / 2),
		DirectX::XMFLOAT2(0, 0), DirectX::XMFLOAT2(1980, 1080), DirectX::XMFLOAT2(0.0f, 0.0f), 0.0f, DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
	UI::Get().sprBlack->End();
	SetDrawBlendMode(BLEND_MODE::NONE);

}

void SceneGame3::Uninit()
{
	for (auto& it : sound)
	{
		delete it;
		it = nullptr;
	}
	EnemyManager::Get().Uninit();
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

	if (Sound::Get().GetSoundType() == Sound::SoundType::Footsteps && Sound::Get().playbackTime <= 1)
	{
		if (rand() % 200 >= 100)
		{
			EnemyManager::Get().GenerateEnemy(Enemy::EnemyType::Haunted, Enemy::Point::R_Haunted);
		}
		else
		{
			EnemyManager::Get().GenerateEnemy(Enemy::EnemyType::Haunted, Enemy::Point::L_Haunted);
		}
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
			// �E
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
			// ��
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