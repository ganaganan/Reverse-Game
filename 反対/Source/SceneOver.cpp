#include "SceneOver.h"

#include <framework.h>
#include "../MyLib/GamePad.h"
#include "FilePath.h"
#include "../MyLib/Blender.h"

void SceneOver::Init()
{
	sprGameOver = std::make_unique<SpriteBatch>(GetTexturePath(TextureAttribute::GameOver), 5, DirectX::XMFLOAT2(0, 0));
	sprTitle = std::make_unique<SpriteBatch>(GetTexturePath(TextureAttribute::Title), 5, DirectX::XMFLOAT2(0, 0));

	// Sound
	sound[SoundType::Shout] = new Audio("Data/Sound/GAMEOVER.wav");
	sound[SoundType::Shout]->Play(false);
//	lastEnemyType = EnemyType::Ghost;
	// Enemy
	switch (lastEnemyType)
	{
	case EnemyType::Daruma:
		enemy = std::make_unique<SkinnedMesh>(FRAMEWORK->GetDevice(), GetModelPath(ModelAttribute::Daruma));
		pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
		break;

	case EnemyType::Ghost:
		enemy = std::make_unique<SkinnedMesh>(FRAMEWORK->GetDevice(), GetModelPath(ModelAttribute::Ghost));
		pos = XMFLOAT3(0.0f, 7.0f, 0.0f);
		break;

	case EnemyType::TotemPole:
		enemy = std::make_unique<SkinnedMesh>(FRAMEWORK->GetDevice(), GetModelPath(ModelAttribute::TotemPole));
		pos = XMFLOAT3(0.0f, -5.0f, -10.0f);
		break;

	default:
		assert(lastEnemyType < EnemyType::EnemyMax && "Enemy! Who are you yanen?!");
		break;
	}
	scale = XMFLOAT3(0.1f, 0.1f, 0.1f);

	// Camera
	eye = XMFLOAT3(0.0f, 15.0f, -50.0f);
	target = XMFLOAT3(0.0f, 15.0f, 0.0f);
	SetFocus(target);
	SetVibration(15.0f, 2);

	Light::TurnOffPointLight(0);
	Light::TurnOffPointLight(1);
	Light::TurnOffPointLight(2);

	// コンスタントバッファ作成
	CreateConstantBuffer(&constantBuffer, sizeof(LightConstantBuffer));

	// Fade
//	textGameover = std::make_unique<Sprite>(FRAMEWORK->GetDevice(), L"./Data/Sprite/gameover.png");
	redScreen = std::make_unique<Sprite>(FRAMEWORK->GetDevice(), L"./Data/Sprite/red_screen.png");
	alpha = 0.0f;

	state = 0;

	isSelectRight = true;
}

void SceneOver::Update()
{
	if (InputTrigger(XINPUT_X))
	{
		switch (nowDay)
		{
		case 0:
			SceneManager::Get().SetScene(SceneManager::MAIN_GAME);	break;
		case 1:
			SceneManager::Get().SetScene(SceneManager::MAIN_GAME2);	break;
		case 2:
			SceneManager::Get().SetScene(SceneManager::MAIN_GAME3);	break;
		}
	}
//	else if (InputTrigger(XINPUT_B))
//	{
//		SceneManager::Get().SetScene(SceneManager::TITLE);
//
//	}

	if (InputTrigger(XINPUT_DPAD_RIGHT))
	{
		isSelectRight = true;
	}
	else if (InputTrigger(XINPUT_DPAD_LEFT))
	{
		isSelectRight = false;
	}

	if (InputTrigger(XINPUT_A))
	{
		if (isSelectRight)
			SceneManager::Get().SetScene(SceneManager::TITLE);
		else
		{
			switch (nowDay)
			{
			case 0:
				SceneManager::Get().SetScene(SceneManager::MAIN_GAME);	break;
			case 1:
				SceneManager::Get().SetScene(SceneManager::MAIN_GAME2);	break;
			case 2:
				SceneManager::Get().SetScene(SceneManager::MAIN_GAME3);	break;
			}
		}
	}

	Light::SetPointLight(2, DirectX::XMFLOAT3(eye.x, eye.y, eye.z), DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f), 500.0f);

	switch (state)
	{
	case 0:
		Vibrate();
		if (timer <= 0)
		{
			state++;
		}
		break;

	case 1:
		Vibrate();
		alpha += 0.017;
		if (alpha >= 1.0f)
		{
			alpha = 1.0f;
			state++;
		}
		break;

	case 2:
		TransitionScenes();
		break;
	}


#ifdef USE_IMGUI
	ImGui::Begin(" game over ");
	ImGui::Text("Retry : X   Title : B");
	ImGui::NewLine();

	ImGui::SliderFloat(" posX ", &pos.x, 0.0f, 100.0f);
	ImGui::SliderFloat(" posY ", &pos.y, -100.0f, 100.0f);
	ImGui::SliderFloat(" posZ ", &pos.z, -100.0f, 0.0f);
	ImGui::NewLine();

	ImGui::SliderFloat(" targetX ", &target.x, -10.0f, 10.0f);
	ImGui::SliderFloat(" targetY ", &target.y, -50.0f, 50.0f);
	ImGui::SliderFloat(" targetZ ", &target.z, -10.0f, 10.0f);

	ImGui::End();
#endif

}

void SceneOver::Render()
{
	// 定数バッファの設定
	SetConstantBuffer();

	XMMATRIX W;
	{
		XMMATRIX S = XMMatrixScaling(scale.x, scale.y, scale.z);
		XMMATRIX R = XMMatrixRotationRollPitchYaw(0.0f, XMConvertToRadians(180.0f), 0.0f);//XMMatrixIdentity();
		XMMATRIX T = XMMatrixTranslation(pos.x, pos.y, pos.z);
		W = S * R * T;
	}
	// view transform matrix
	XMMATRIX V;
	{
		XMVECTOR E = XMVectorSet(eye.x, eye.y, eye.z, 1.0f);
		XMVECTOR F = XMVectorSet(focus.x, focus.y, focus.z, 1.0f);
		XMVECTOR U = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
		V = XMMatrixLookAtLH(E, F, U);
	}
	// perspective projection matrix
	XMMATRIX P = Camera::Get().GetProjectionMatrix();

	XMFLOAT4X4 wvp;
	XMFLOAT4X4 world;

	XMStoreFloat4x4(&wvp, W * V * P);
	XMStoreFloat4x4(&world, W);

	enemy->Render(FRAMEWORK->GetDeviceContext(), wvp, world, XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));



	static int time = 0;
	static float posY;
	time += 2;
	posY = sinf( time * 0.01745f) * 50;
	if (time >= 360)time = 0;

	SetDrawBlendMode(BLEND_MODE::ALPHA);
	redScreen->Render(FRAMEWORK->GetDeviceContext(), 0.0f, 0.0f, 1980.0f, 1080.0f, 0.0f, 0.0f, 1980.0f, 1080.0f, 0.0f, 1.0f, 1.0f, 1.0f, alpha);
	if (state == 2)
	{
		sprGameOver->Begin();
		sprGameOver->Draw(550,400, 869, 128, 0, 0, 869, 128, 0, 0, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f);//げーむおーばー
		sprGameOver->End();

		sprTitle->Begin();
		if (isSelectRight)
		{
			sprTitle->Draw(550, 700, 229, 58, 576, 128, 229, 58, 0, 0, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f);//リトライ
			sprTitle->Draw(1050, 700 + posY, 310, 58, 576, 192, 310, 58, 0, 0, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f);//タイトル
		}
		else
		{
			sprTitle->Draw(550, 700 + posY, 229, 58, 576, 128, 229, 58, 0, 0, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f);//リトライ
			sprTitle->Draw(1050, 700, 310, 58, 576, 192, 310, 58, 0, 0, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f);//タイトル
		}
		sprTitle->End();
	}
	SetDrawBlendMode(BLEND_MODE::NONE);
}

void SceneOver::Uninit()
{
	for (auto& it : sound)
	{
		delete it;
		it = nullptr;
	}
}

// 振動カメラ
void SceneOver::Vibrate()
{
	//	if (timer > 0)
	{
		timer--;

		XMFLOAT3 focus(0, 0, 0);

		focus.x = static_cast<float>(rand() % 30) / 100 * range + target.x;
		focus.y = static_cast<float>(rand() % 30) / 100 * range + target.y;

		SetFocus(focus);
	}
}

// シーン遷移
void SceneOver::TransitionScenes()
{
	if (InputTrigger(XINPUT_X))
	{
		switch (nowDay)
		{
		case 0:
			SceneManager::Get().SetScene(SceneManager::MAIN_GAME);	break;
		case 1:
			SceneManager::Get().SetScene(SceneManager::MAIN_GAME2);	break;
		case 2:
			SceneManager::Get().SetScene(SceneManager::MAIN_GAME3);	break;
		}
	}
	else if (InputTrigger(XINPUT_B))
	{
		SceneManager::Get().SetScene(SceneManager::TITLE);
	}
}