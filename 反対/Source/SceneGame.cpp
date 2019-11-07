#include "SceneGame.h"

#include <DirectXMath.h>
#include <framework.h>

#include "Camera.h"
#include "Stage.h"
#include "Light.h"

DirectX::XMFLOAT3 pos{ 0.0f,0.0f,0.0f };

void SceneGame::Init()
{
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
	ImGui::Begin("pos");
	ImGui::SliderFloat("x", &pos.x, -200.0f, 200.0f);
	ImGui::SliderFloat("y", &pos.y, -200.0f, 200.0f);
	ImGui::SliderFloat("z", &pos.z, -200.0f, 200.0f);
	ImGui::End();
#endif
	Light::SetPointLight(0, DirectX::XMFLOAT3(-50.0f, -7.0f, 27.0f), DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f), 50.0f);
	Light::SetPointLight(1, DirectX::XMFLOAT3(50.0f, -7.0f, 27.0f), DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f), 50.0f);

	Camera::Get().Update();
	Stage::Get().Update();


}

void SceneGame::Render()
{
	using namespace DirectX;

	XMMATRIX V, P;
	V =	Camera::Get().GetViewMatrix();
	P =	Camera::Get().GetProjectionMatrix();

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


	Stage::Get().Render(V, P);
}

void SceneGame::Uninit()
{

}