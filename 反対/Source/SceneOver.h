#pragma once

#include "Scene.h"
#include "../MyLib/Audio.h"
#include "Light.h"
#include "../MyLib/sprite.h"

class SceneOver : public BaseScene
{
	enum SoundType
	{
		Shout,
		Max,
	};
	Audio* sound[SoundType::Max];


	// Enemy
	enum EnemyType
	{
		Daruma,
		Ghost,
		TotemPole,
		EnemyMax,
	};
	std::unique_ptr<SkinnedMesh> enemy;

	XMFLOAT3	pos;
	XMFLOAT3	scale;

	// Camera
	XMFLOAT3	eye;
	XMFLOAT3	focus;
	XMFLOAT3	target;
	int			timer = 0;				// 振動時間
	float		range = 0.0f;			// 振動幅

	// Fade
//	std::unique_ptr<Sprite> textGameover;
	std::unique_ptr<Sprite> redScreen;
	std::unique_ptr<SpriteBatch> sprGameOver;
	std::unique_ptr<SpriteBatch> sprTitle;
	float		alpha;

	int			state;

	bool		isSelectRight;

public:
	SceneOver() {}
	~SceneOver() { Uninit(); }

	void Init();
	void Uninit();
	void Update();
	void Render();

private:
	void	TransitionScenes();

	void	Vibrate();		// 振動カメラ
	void	SetVibration(float r, float t)		// 振動開始
	{
		range = r;
		timer = t * 60;
	}
	void	SetFocus(const XMFLOAT3 _focus)
	{
		focus = _focus;
	}

private:

	struct LightConstantBuffer
	{
		DirectX::XMFLOAT4	lightColor;
		DirectX::XMFLOAT4	lightDir;
		DirectX::XMFLOAT4	ambientColor;
		DirectX::XMFLOAT4	eyePos;

		PointLight		pointLight[Light::POINT_MAX];
	};

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

extern SceneOver sceneOver;