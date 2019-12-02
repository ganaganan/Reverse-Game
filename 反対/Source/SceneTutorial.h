#pragma once

#include <Windows.h>
#include <framework.h>
#include "BaseScene.h"
#include "Light.h"
#include "Camera.h"
#include "Enemy.h"

#define USE_NEW

class SceneTutorial : public BaseScene
{
	struct LightConstantBuffer
	{
		DirectX::XMFLOAT4	lightColor;
		DirectX::XMFLOAT4	lightDir;
		DirectX::XMFLOAT4	ambientColor;
		DirectX::XMFLOAT4	eyePos;

		PointLight		pointLight[Light::POINT_MAX];
	};

#ifndef USE_NEW
	enum TutorialState
	{
		OpenEye,
		See,
		MoveLeft,
		LeftPeek,
		Back,
		MoveRight,
		RightPeek,
		Back2,
		Start,
	};
	TutorialState state;
#endif
public:
	enum TutorialState
	{
		OpenEye,
		FellOverLeft,
		OnLight,
		OffLight,
		Back,
		FellOverRight,
		KillGhost,
		OffLight2,
		Back2,
		WarningSound,
		Start,
	};
	static TutorialState state;
	static bool endOfPeek;
	static bool endOfLeft;
	static bool endOfRight;

private:
	unsigned int time;
#pragma region OPEN EYE
	DirectX::XMFLOAT2 openEyeBlackUp;
	DirectX::XMFLOAT2 openEyeBlackDown;
#pragma endregion

	Enemy sandBag;
	bool isBackHome;

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

	void SetConstantBuffer();

public:
	void Init();
	void Uninit();
	void Update();
	void Render();


	void UpdateOpenEye();
	void UpdateFellOverLeft();
	void UpdateOnLight();
	void UpdateOffLight();
	void UpdateFellOverRight();
	void UpdateBack();
	void UpdateKillGhost();
	void UpdateOffLight2();
	void UpdateBack2();
	void UpdateWarningSound();
	void UpdateStart();

	void DrawOpenEye();
	void DrawFellOverLeft();
	void DrawOnLight();
	void DrawOffLight(DirectX::XMMATRIX& _V, DirectX::XMMATRIX& _P);
	void DrawFellOverRight(DirectX::XMMATRIX& _V, DirectX::XMMATRIX& _P);
	void DrawBack(DirectX::XMMATRIX& _V, DirectX::XMMATRIX& _P);
	void DrawKillGhost(DirectX::XMMATRIX& _V, DirectX::XMMATRIX& _P);
	void DrawOffLight2();
	void DrawBack2();
	void DrawWarningSound();
	void DrawStart();
};

extern SceneTutorial sceneTutorial;