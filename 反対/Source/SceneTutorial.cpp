#include "SceneTutorial.h"

#include "Stage.h"
#include "Camera.h"
#include "Scene.h"
#include "../MyLib/GamePad.h"
#include "UI.h"

bool SceneTutorial::endOfPeek;
bool SceneTutorial::endOfLeft;
bool SceneTutorial::endOfRight;
SceneTutorial::TutorialState SceneTutorial::state;


void SceneTutorial::Init()
{
	// コンスタントバッファ作成
	CreateConstantBuffer(&constantBuffer, sizeof(LightConstantBuffer));

	Stage::Get().Init();
	UI::Get().Init();
	sandBag.Init();
	EnemyManager::Get().Init();
	sandBag.Generate(Enemy::EnemyType::Daruma, Enemy::R_Point1);
	Camera::Get().Init();
	DirectX::XMFLOAT3(-38.0f, 25.0f, 55.0f);
	DirectX::XMFLOAT3(38.0f, 25.0f, 55.0f);
	DirectX::XMFLOAT3(0.0f, 60.0f, 2.0f);
	Light::battery = 16200;

	Light::SetPointLight(0, DirectX::XMFLOAT3(-38.0f, 25.0f, 55.0f), DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f), 45.0f);	// 左
	Light::SetPointLight(1, DirectX::XMFLOAT3(38.0f, 25.0f, 55.0f), DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f), 45.0f);	// 右
	Light::SetPointLight(2, DirectX::XMFLOAT3(0.0f, 60.0f, 2.0f), DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f), 110.0f);	// 真ん中
	Light::SetPointLight(3, DirectX::XMFLOAT3(-38.0f, 25.0f, 55.0f), DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f), 95.0f);	// 左 手動
	Light::SetPointLight(4, DirectX::XMFLOAT3(38.0f, 25.0f, 55.0f), DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f), 95.0f);	// 右 手動
	Light::TurnOffPointLight(3);
	Light::TurnOffPointLight(4);
	Light::isEnableBattery = true;

//	Light::TurnOffPointLight(0);
//	Light::TurnOffPointLight(1);

#ifndef USE_NEW
	state = TutorialState::OpenEye;
#endif
	time = 0;
#pragma region OPEN EYE
	openEyeBlackUp = DirectX::XMFLOAT2(0.0f,0.0f);
	openEyeBlackDown = DirectX::XMFLOAT2(0.0f, framework::SCREEN_HEIGHT / 2);
	state = TutorialState::OpenEye;
#pragma endregion
	isBackHome = false;
	endOfPeek = false;
	endOfLeft = false;
	endOfRight = false;
}

void SceneTutorial::Update()
{
#ifdef USE_IMGUI
	ImGui::Begin("Scene Tutorial");
	ImGui::Text("Next : A");
	ImGui::End();
#endif

#ifndef USE_NEW
	switch (state)
	{
	case SceneTutorial::OpenEye:
		OpenEye_f();
		break;
	case SceneTutorial::See:
		See_f();
		break;
	case SceneTutorial::MoveLeft:
		MoveLeft_f();
		break;
	case SceneTutorial::LeftPeek:
		LeftPeek_f();
		break;
	case SceneTutorial::Back:
		Back_f();
		break;
	case SceneTutorial::MoveRight:
		MoveRight_f();
		break;
	case SceneTutorial::RightPeek:
		RightPeek_f();
		break;
	case SceneTutorial::Back2:
		Back2_f();
		break;
	case SceneTutorial::Start:
		break;
	default:
		break;
	}
#endif

	switch (state)
	{
	case SceneTutorial::OpenEye:
		UpdateOpenEye();
		break;
	case SceneTutorial::FellOverLeft:
		UpdateFellOverLeft();
		break;
	case SceneTutorial::OnLight:
		UpdateOnLight();
		break;
	case SceneTutorial::OffLight:
		UpdateOffLight();
		break;
	case SceneTutorial::Back:
		UpdateBack();
		break;
	case SceneTutorial::FellOverRight:
		UpdateFellOverRight();
		break;
	case SceneTutorial::KillGhost:
		UpdateKillGhost();
		break;
	case SceneTutorial::OffLight2:
		UpdateOffLight2();
		break;
	case SceneTutorial::Back2:
		UpdateBack2();
		break;
	case SceneTutorial::WarningSound:
		UpdateWarningSound();
		break;
	case SceneTutorial::Start:
		UpdateStart();
		break;
	default:
		break;
	}

//	Camera::Get().Update(isBackHome);
	Light::Update();
	UI::Get().Update();
	sandBag.JudgeIsHitLight();
}

void SceneTutorial::Render()
{
	// 定数バッファの作成
	SetConstantBuffer();

	using namespace DirectX;

	// 行列設定
	XMMATRIX V, P;
	V = Camera::Get().GetViewMatrix();
	P = Camera::Get().GetProjectionMatrix();

	Stage::Get().Render(V, P);


	SetDrawBlendMode(BLEND_MODE::ALPHA);

	switch (state)
	{
	case SceneTutorial::OpenEye:
		DrawOpenEye();
		break;
	case SceneTutorial::FellOverLeft:
		DrawFellOverLeft();
		break;
	case SceneTutorial::OnLight:
		DrawOnLight();
		break;
	case SceneTutorial::OffLight:
		DrawOffLight(V, P);
		break;
	case SceneTutorial::Back:
		DrawBack(V, P);
		break;
	case SceneTutorial::FellOverRight:
		DrawFellOverRight(V, P);
		break;
	case SceneTutorial::KillGhost:
		DrawKillGhost(V, P);
		break;
	case SceneTutorial::OffLight2:
		DrawOffLight2();
		break;
	case SceneTutorial::Back2:
		DrawBack2();
		break;
	case SceneTutorial::WarningSound:
		DrawWarningSound();
		break;
	case SceneTutorial::Start:
		DrawStart();
		break;
	default:
		break;
	}

	SetDrawBlendMode(BLEND_MODE::NONE);
}

void SceneTutorial::Uninit()
{
	UI::Get().Uninit();
}

void SceneTutorial::SetConstantBuffer()
{
	// 定数バッファ設定
	LightConstantBuffer cb;
	cb.ambientColor = Light::ambient;
	cb.lightDir = Light::lightDir;
	cb.lightColor = Light::dirLightColor;
	cb.eyePos.x = Camera::Get().GetPos().x;
	cb.eyePos.y = Camera::Get().GetPos().y;
	cb.eyePos.z = Camera::Get().GetPos().z;
	cb.eyePos.w = 1.0f;
	memcpy(cb.pointLight, Light::pointLight, sizeof(PointLight) * Light::POINT_MAX);

	FRAMEWORK->GetDeviceContext()->UpdateSubresource(constantBuffer, 0, NULL, &cb, 0, 0);
	FRAMEWORK->GetDeviceContext()->VSSetConstantBuffers(2, 1, &constantBuffer);
	FRAMEWORK->GetDeviceContext()->PSSetConstantBuffers(2, 1, &constantBuffer);
}


#ifndef USE_NEW
void SceneTutorial::OpenEye_f()
{
	if (time++ <= 18/*0.3*/)
	{
		openEyeBlackUp.y -= (framework::SCREEN_HEIGHT / 2) * time / 18;
		openEyeBlackDown.y += (framework::SCREEN_HEIGHT / 2) * time / 18;
	}
	else
	{
		state = TutorialState::MoveLeft;
	}
}

void SceneTutorial::See_f()
{

}

void SceneTutorial::MoveLeft_f()
{
	Camera::Get().Update();
}

void SceneTutorial::LeftPeek_f()
{

}

void SceneTutorial::Back_f()
{

}

void SceneTutorial::MoveRight_f()
{

}

void SceneTutorial::RightPeek_f()
{

}

void SceneTutorial::Back2_f()
{

}

#endif


#pragma region Updates
void SceneTutorial::UpdateOpenEye()
{
	if (time++ <= 30/*0.5*/)
	{
		openEyeBlackUp.y -= (framework::SCREEN_HEIGHT / 2) * 1 / 30;
		openEyeBlackDown.y += (framework::SCREEN_HEIGHT / 2) * 1 / 30;
	}
	else
	{
		time = 0;
		state = TutorialState::FellOverLeft;
	}

}

void SceneTutorial::UpdateFellOverLeft()
{
	if (InputTrigger(XINPUT_X))
	{
		SceneManager::Get().SetScene(SceneManager::MAIN_GAME);
	}

	Camera::Get().UpdateTutorial();
	if (endOfPeek)
	{
		state = TutorialState::OnLight;
		endOfPeek = false;
	}
}

void SceneTutorial::UpdateOnLight()
{
	Camera::Get().UpdateTutorial();
	if (InputTrigger(XINPUT_A))
	{
		state = TutorialState::OffLight;
	}
}

void SceneTutorial::UpdateOffLight()
{
	Camera::Get().UpdateTutorial();
	if (InputTrigger(XINPUT_A))
	{
		state = TutorialState::Back;
	}

}

void SceneTutorial::UpdateBack()
{
	Camera::Get().UpdateTutorial();
	if (endOfLeft)
	{
		state = TutorialState::FellOverRight;
		endOfLeft = false;
	}

}

void SceneTutorial::UpdateFellOverRight()
{
	Camera::Get().UpdateTutorial();
	if (endOfPeek)
	{
		endOfPeek = false;
		state = TutorialState::KillGhost;
	}
}

void SceneTutorial::UpdateKillGhost()
{
	sandBag.JudgeIsHitLight();
	Camera::Get().UpdateTutorial();
	if (!sandBag.GetEnable())
	{
		state = OffLight2;
	}
}

void SceneTutorial::UpdateOffLight2()
{
	Camera::Get().UpdateTutorial();
	if (InputTrigger(XINPUT_A))
	{
		state = Back2;
	}
}

void SceneTutorial::UpdateBack2()
{
	Camera::Get().UpdateTutorial();
	if (endOfRight)
	{
		state = WarningSound;
	}
}

void SceneTutorial::UpdateWarningSound()
{
	if (time++ >= 180)
	{
		state = Start;
	}
}

void SceneTutorial::UpdateStart()
{
	SceneManager::Get().SetScene(SceneManager::MAIN_GAME);
}


#pragma endregion

#pragma region Draws
void SceneTutorial::DrawOpenEye()
{
	UI::Get().sprBlack->Begin();
	UI::Get().sprBlack->Draw(0.0f, openEyeBlackUp.y, 1980, framework::SCREEN_HEIGHT / 2, 0.0f, 0.0f, 1980.0f, 1080.0f, 0, 0, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f);
	UI::Get().sprBlack->Draw(0.0f, openEyeBlackDown.y, 1980, framework::SCREEN_HEIGHT / 2, 0.0f, 0.0f, 1980.0f, 1080.0f, 0, 0, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f);
	UI::Get().sprBlack->End();
}

void SceneTutorial::DrawFellOverLeft()
{
	static float s_alpha = 0.5f;
	static bool isUp = true;
	switch (isUp)
	{
	case true:
		s_alpha += 0.01f;
		if (s_alpha >= 1.0f)
		{
			s_alpha = 1.0f;
			isUp = false;
		}
		break;
	case false:
		s_alpha -= 0.01f;
		if (s_alpha <= 0.5f)
		{
			s_alpha = 0.5f;
			isUp = true;
		}
		break;
	}

	UI::Get().sprTutorial->Begin();
	UI::Get().sprTutorial->Draw(400, 700, 208, 108, 0, 384, 208, 108, 0, 0, 0.0f, 1.0f, 1.0f, 1.0f, s_alpha);	// ←
	UI::Get().sprTutorial->Draw(500, 500, 448, 120, 0, 0, 448, 120, 0, 0, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f);	// 左スティックで移動
	UI::Get().sprTutorial->End();
}

void SceneTutorial::DrawOnLight()
{
	static float s_alpha = 0.5f;
	static bool isUp = true;
	switch (isUp)
	{
	case true:
		s_alpha += 0.01f;
		if (s_alpha >= 1.0f)
		{
			s_alpha = 1.0f;
			isUp = false;
		}
		break;
	case false:
		s_alpha -= 0.01f;
		if (s_alpha <= 0.5f)
		{
			s_alpha = 0.5f;
			isUp = true;
		}
		break;
	}

	UI::Get().sprTutorial->Begin();
	UI::Get().sprTutorial->Draw(650, 500, 606, 64, 0, 128, 606, 64, 0, 0, 0.0f, 1.0f, 1.0f, 1.0f, s_alpha);	// ライトON
	UI::Get().sprTutorial->End();
}

void SceneTutorial::DrawOffLight(DirectX::XMMATRIX& _V, DirectX::XMMATRIX& _P)
{
	sandBag.Render(_V, _P);

	static float s_alpha = 0.5f;
	static bool isUp = true;
	switch (isUp)
	{
	case true:
		s_alpha += 0.01f;
		if (s_alpha >= 1.0f)
		{
			s_alpha = 1.0f;
			isUp = false;
		}
		break;
	case false:
		s_alpha -= 0.01f;
		if (s_alpha <= 0.5f)
		{
			s_alpha = 0.5f;
			isUp = true;
		}
		break;
	}

	UI::Get().sprTutorial->Begin();
	UI::Get().sprTutorial->Draw(300, 600, 1332, 68, 0, 288, 1332, 68, 0, 0, 0.0f, 1.0f, 1.0f, 1.0f, s_alpha);	// バッテリー消費がうんぬん
	UI::Get().sprTutorial->Draw(642, 900, 636, 64, 0, 192, 636, 64, 0, 0, 0.0f, 1.0f, 1.0f, 1.0f, s_alpha);	// ライトOFF
	UI::Get().sprTutorial->End();

	UI::Get().Render();
}

void SceneTutorial::DrawBack(DirectX::XMMATRIX& _V, DirectX::XMMATRIX& _P)
{
	sandBag.Render(_V, _P);


	UI::Get().Render();
}

void SceneTutorial::DrawFellOverRight(DirectX::XMMATRIX& _V, DirectX::XMMATRIX& _P)
{
	sandBag.Render(_V, _P);


	static float s_alpha = 0.5f;
	static bool isUp = true;
	switch (isUp)
	{
	case true:
		s_alpha += 0.01f;
		if (s_alpha >= 1.0f)
		{
			s_alpha = 1.0f;
			isUp = false;
		}
		break;
	case false:
		s_alpha -= 0.01f;
		if (s_alpha <= 0.5f)
		{
			s_alpha = 0.5f;
			isUp = true;
		}
		break;
	}
	UI::Get().sprTutorial->Begin();
	UI::Get().sprTutorial->Draw(1400, 700, 208, 108, 224, 384, 208, 108, 0, 0, 0.0f, 1.0f, 1.0f, 1.0f, s_alpha);	// →
	UI::Get().sprTutorial->End();

	UI::Get().Render();

}


void SceneTutorial::DrawKillGhost(DirectX::XMMATRIX& _V, DirectX::XMMATRIX& _P)
{
	sandBag.Render(_V, _P);

	static float s_alpha = 0.5f;
	static bool isUp = true;
	switch (isUp)
	{
	case true:
		s_alpha += 0.01f;
		if (s_alpha >= 1.0f)
		{
			s_alpha = 1.0f;
			isUp = false;
		}
		break;
	case false:
		s_alpha -= 0.01f;
		if (s_alpha <= 0.5f)
		{
			s_alpha = 0.5f;
			isUp = true;
		}
		break;
	}
	UI::Get().sprTutorial->Begin();
	UI::Get().sprTutorial->Draw(300,600,827,69,480,0,827,69,0,0,0.0f,1.0f,1.0f,1.0f,s_alpha);	// おばけにライト
	UI::Get().sprTutorial->End();

	UI::Get().Render();

}

void SceneTutorial::DrawOffLight2()
{
	static float s_alpha = 0.5f;
	static bool isUp = true;
	switch (isUp)
	{
	case true:
		s_alpha += 0.01f;
		if (s_alpha >= 1.0f)
		{
			s_alpha = 1.0f;
			isUp = false;
		}
		break;
	case false:
		s_alpha -= 0.01f;
		if (s_alpha <= 0.5f)
		{
			s_alpha = 0.5f;
			isUp = true;
		}
		break;
	}

	UI::Get().sprTutorial->Begin();
	UI::Get().sprTutorial->Draw(400, 600, 636, 64, 0, 192, 636, 64, 0, 0, 0.0f, 1.0f, 1.0f, 1.0f, s_alpha);
	UI::Get().sprTutorial->End();

	UI::Get().Render();

}

void SceneTutorial::DrawBack2()
{
	UI::Get().Render();

}

void SceneTutorial::DrawWarningSound()
{
	static int time = 0;

	if (++time % 60 >= 30)
	{
		UI::Get().sprTutorial->Begin();
		UI::Get().sprTutorial->Draw(800, 500, 257, 68, 448, 384, 257, 68, 0, 0, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f);	// 音に注意
		UI::Get().sprTutorial->End();
	}
	UI::Get().Render();

}

void SceneTutorial::DrawStart()
{
//	UI::Get().sprTutorial->Begin();
//	UI::Get().sprTutorial->Draw(800, 500, 257, 68, 448, 384, 257, 68, 0, 0, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f);	// 音に注意
//	UI::Get().sprTutorial->End();
	UI::Get().Render();

}

#pragma endregion