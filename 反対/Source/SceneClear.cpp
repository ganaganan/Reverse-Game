#include "SceneClear.h"

#include <framework.h>

#include "../MyLib/GamePad.h"
#include "UI.h"
#include "Camera.h"
#include "Stage.h"
#include "FilePath.h"

void SceneClear::Init()
{
	// コンスタントバッファ作成
	CreateConstantBuffer(&constantBuffer, sizeof(LightConstantBuffer));

	sprClear = std::make_unique<SpriteBatch>(GetTexturePath(TextureAttribute::GameOver), 5, DirectX::XMFLOAT2(0, 0));
	sprString = std::make_unique<SpriteBatch>(GetTexturePath(TextureAttribute::Title), 5, DirectX::XMFLOAT2(0,0));

	alpha = 0.0f;
	time = 0;
	isalpha = false;
}

void SceneClear::Update()
{
#ifdef USE_IMGUI
	ImGui::Begin("Scene Clear");

	ImGui::Text("Go Title : A");

	ImGui::End();
#endif 

	if (InputTrigger(XINPUT_A))
	{
		SceneManager::Get().SetScene(SceneManager::TITLE);
	}
}

void SceneClear::Render()
{
	SetConstantBuffer();

	using namespace DirectX;

	// 行列設定
	XMMATRIX V, P;
	V = Camera::Get().GetViewMatrix();
	P = Camera::Get().GetProjectionMatrix();

	Stage::Get().Render(V, P);


	alpha += 0.01f;
	if (alpha >= 1.0f)
	{
		alpha = 1.0f;
		isalpha = true;
	}

	if (isalpha)
	{
		time++;
		if (time >= 60)time = 60;
	}

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


	SetDrawBlendMode(BLEND_MODE::ALPHA);
	UI::Get().sprWhite->Begin();
	UI::Get().sprWhite->Draw(0, 0, 1980, 1080, 0, 0, 1980, 1080, 0, 0, 0.0f, 1.0f, 1.0f, 1.0f, alpha);
	UI::Get().sprWhite->End();
	sprClear->Begin();
	sprClear->Draw(550, 500, 798, 103, 0, 128, 798, 103, 0, 0, 0.0f, 1.0f, 1.0f, 1.0f, alpha);
	sprClear->End();

	if (time == 60)
	{
		sprString->Begin();
		sprString->Draw(670, 800, 564, 64, 0, 192, 564, 64, 0, 0, 0.0f, 1.0f, 1.0f, 1.0f, s_alpha);
		sprString->End();
	}
	SetDrawBlendMode(BLEND_MODE::NONE);
}

void SceneClear::Uninit()
{

}