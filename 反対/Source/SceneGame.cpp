#include "SceneGame.h"

#include <DirectXMath.h>
#include <framework.h>

#include "Camera.h"
#include "Stage.h"
#include "Light.h"


void SceneGame::Init()
{
	// デバッグ用
	debugLightPos[0] = DirectX::XMFLOAT3(-50.0f, -7.0f, 27.0f);
	debugLightPos[1] = DirectX::XMFLOAT3(50.0f, -7.0f, 27.0f);
	debugLightPos[2] = DirectX::XMFLOAT3(0.0f, -26.0f, 63.0f);

	debugLightRange[0] = 50.0f;
	debugLightRange[1] = 50.0f;
	debugLightRange[2] = 150.0f;

	// コンスタントバッファ作成
	CreateConstantBuffer(&constantBuffer, sizeof(LightConstantBuffer));

	Stage::Get().Init();

	// ライトの設定
	Light::SetAmbient(DirectX::XMFLOAT3(0.1f, 0.1f, 0.1f));

	lightDir.x = sinf(DirectX::XM_PI);
	lightDir.y = -0.4f;
	lightDir.z = cosf(DirectX::XM_PI);
	Light::SetDirLight(lightDir, DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f));


}

void SceneGame::Update()
{	
#ifdef USE_IMGUI
	UseImGui();
#endif
	// ポイントライトの設定
	Light::SetPointLight(0, debugLightPos[0]/*DirectX::XMFLOAT3(-50.0f, -7.0f, 27.0f)*/,	DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f), debugLightRange[0]);	// 左
	Light::SetPointLight(1, debugLightPos[1]/*DirectX::XMFLOAT3(50.0f, -7.0f, 27.0f)*/,		DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f), debugLightRange[1]);	// 右
	Light::SetPointLight(2, debugLightPos[2]/*DirectX::XMFLOAT3(0.0f, -26.0f, 63.0f)*/,		DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f), debugLightRange[2]);	// 真ん中

	Camera::Get().Update();
	Stage::Get().Update();
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
}

void SceneGame::Uninit()
{

}


void SceneGame::UseImGui()
{
	ImGui::Begin("SceneGame");

	if (ImGui::TreeNode("PointLight Status"))
	{
		if (ImGui::TreeNode(" No.1 "))
		{
			ImGui::SliderFloat3("Position ", &debugLightPos[0].x, -200.0f, 200.0f);
			ImGui::SliderFloat("Range ", &debugLightRange[0], 0.1f, 1000.0f);
			ImGui::TreePop();
		}
		if (ImGui::TreeNode(" No.2 "))
		{
			ImGui::SliderFloat3("Position ", &debugLightPos[1].x, -200.0f, 200.0f);
			ImGui::SliderFloat("Range ", &debugLightRange[1], 0.1f, 1000.0f);
			ImGui::TreePop();
		}
		if (ImGui::TreeNode(" No.3 "))
		{
			ImGui::SliderFloat3("Position ", &debugLightPos[2].x, -200.0f, 200.0f);
			ImGui::SliderFloat("Range ", &debugLightRange[2], 0.1f, 1000.0f);
			ImGui::TreePop();
		}

		ImGui::TreePop();
	}

	ImGui::End();

}