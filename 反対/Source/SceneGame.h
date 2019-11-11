#pragma once

#include <DirectXMath.h>
#include <Windows.h>
#include <d3d11.h>
#include <framework.h>

#include "Scene.h"
#include "Camera.h"
#include "Light.h"

class SceneGame : public BaseScene
{
	struct LightConstantBuffer
	{
		DirectX::XMFLOAT4	lightColor;
		DirectX::XMFLOAT4	lightDir;
		DirectX::XMFLOAT4	ambientColor;
		DirectX::XMFLOAT4	eyePos;

		PointLight		pointLight[Light::POINT_MAX];
	};

public:
	/*----------------------------------------*/
	// Degub variable and function
	DirectX::XMFLOAT3	debugLightPos[Light::POINT_MAX];
	float				debugLightRange[Light::POINT_MAX];
	void UseImGui();
	/*----------------------------------------*/

public:
	SceneGame(){}
	void Init();
	void Uninit();
	void Update();
	void Render();

protected:
	ID3D11Buffer* constantBuffer;
	bool CreateConstantBuffer(ID3D11Buffer** ppCB, u_int size)
	{
		// 定数バッファ生成
		D3D11_BUFFER_DESC bd;
		ZeroMemory(&bd, sizeof(D3D11_BUFFER_DESC));
		bd.ByteWidth = size;
		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		bd.CPUAccessFlags = 0;

		HRESULT hr = FRAMEWORK->GetDevice()->CreateBuffer(&bd, NULL, ppCB);
		assert(SUCCEEDED(hr));

		return true;
	}

	void SetConstantBuffer()
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
};

extern SceneGame sceneGame;