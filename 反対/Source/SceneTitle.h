#pragma once

#include <memory>
#include <framework.h>
#include "Scene.h"
#include "../MyLib/Audio.h"
#include "Light.h"
#include "Camera.h"

class SceneTitle : public BaseScene
{
	static constexpr int FADE_TIME = 60 * 1;

	//enum SoundTypeDebug
	//{
	//	Rain,
	//	Thunder_title,
	//	Thunderstorm,
	//	Chaim,
	//	OpenDoor,
	//	Rain2,
	//	Pass,
	//	RingTone,
	//	Wind,
	//	Walk,
	//	Thunder_game,
	//	Daruma,
	//	Ghost,
	//	pole,
	//	Max,
	//};
	//Audio*  debugSound[SoundTypeDebug::Max];
	//float	debugSoundVol[SoundTypeDebug::Max];

	std::unique_ptr<SkinnedMesh> stageModel;
	std::unique_ptr<SkinnedMesh> rightDoorModel;
	std::unique_ptr<SkinnedMesh> leftDoorModel;

	std::unique_ptr<SpriteBatch> sprTitle;

	struct LightConstantBuffer
	{
		DirectX::XMFLOAT4	lightColor;
		DirectX::XMFLOAT4	lightDir;
		DirectX::XMFLOAT4	ambientColor;
		DirectX::XMFLOAT4	eyePos;

		PointLight		pointLight[Light::POINT_MAX];
	};

	enum SoundType
	{
		Rain,			// 雨
		Thunder,		// 雷
		Thunderstorm,	// 雷雨
		Max,
	};
	Audio*	sound[SoundType::Max];
	int		soundType;		// SoundType - Rain or Thunderstorm

	int		state;
	float	fadePosYUp;
	float	fadePosYDown;
	bool	goTuto;

public:
	SceneTitle(){}

	void Init();
	void Uninit();
	void Update();
	void Render();

	void PlaySoundEnviroment();

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

extern SceneTitle sceneTitle;