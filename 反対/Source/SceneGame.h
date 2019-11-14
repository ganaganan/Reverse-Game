#pragma once

#include <DirectXMath.h>
#include <Windows.h>
#include <d3d11.h>
#include <framework.h>
#include <sprite.h>

#include "Scene.h"
#include "Camera.h"
#include "Light.h"
#include "../MyLib/Audio.h"

#pragma region Day1
class SceneGame : public BaseScene
{
	int PLAY_SOUND_MIN;
	int PLAY_SOUND_MAX;
	int SOUND_VOLUME_MIN;
	int SOUND_VOLUME_MAX;
	int thisTimePlaySound;
	int SoundVol;

	enum SoundType
	{
		Chaim,
		OpenDoor,
		Rain,
		Pass,
		RingTone,
		Wind,
		Walk,
		Thunder,
		Max,
	};
	Audio* sound[SoundType::Max];
	float  soundVol[SoundType::Max];

private:
	// エネミーの生成間隔
	int GENERATE_ENEMY_MIN;
	int GENERATE_ENEMY_MAX;
	// 敵ごとの生成確率
	int GENERATE_PER_ENEMY1;
	int GENERATE_PER_ENEMY2;
	int GENERATE_PER_ENEMY3;
	int generateEnemyCount;
	int thisTimeGeneratePerCount;

	struct LightConstantBuffer
	{
		DirectX::XMFLOAT4	lightColor;
		DirectX::XMFLOAT4	lightDir;
		DirectX::XMFLOAT4	ambientColor;
		DirectX::XMFLOAT4	eyePos;

		PointLight		pointLight[Light::POINT_MAX];
	};

private:			
	//				  FPS  sec  minute
	int	TOTAL_NIGHTS = 60 * 60 * 1;
	int nightTime;

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

	void GenerateEnemy();
	void JudgeGoNextDay();
	void PlayEnvironment();
};


#pragma endregion

#pragma  region Day2
class SceneGame2 : public BaseScene
{
private:
	// エネミーの生成間隔
	int GENERATE_ENEMY_MIN;
	int GENERATE_ENEMY_MAX;
	// 敵ごとの生成確率
	int GENERATE_PER_ENEMY1;
	int GENERATE_PER_ENEMY2;
	int GENERATE_PER_ENEMY3;
	int generateEnemyCount;
	int thisTimeGeneratePerCount;


	int PLAY_SOUND_MIN;
	int PLAY_SOUND_MAX;
	int SOUND_VOLUME_MIN;
	int SOUND_VOLUME_MAX;
	int thisTimePlaySound;
	int SoundVol;

	enum SoundType
	{
		Chaim,
		OpenDoor,
		Rain,
		Pass,
		RingTone,
		Wind,
		Walk,
		Thunder,
		Max,
	};
	Audio* sound[SoundType::Max];
	float  soundVol[SoundType::Max];

	struct LightConstantBuffer
	{
		DirectX::XMFLOAT4	lightColor;
		DirectX::XMFLOAT4	lightDir;
		DirectX::XMFLOAT4	ambientColor;
		DirectX::XMFLOAT4	eyePos;

		PointLight		pointLight[Light::POINT_MAX];
	};

private:
	//				  FPS  sec  minute
	int	TOTAL_NIGHTS = 60 * 60 * 1;
	int nightTime;


public:
	/*----------------------------------------*/
	// Degub variable and function
	DirectX::XMFLOAT3	debugLightPos[Light::POINT_MAX];
	float				debugLightRange[Light::POINT_MAX];
	void UseImGui();
	/*----------------------------------------*/

public:
	SceneGame2() {}
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

	void GenerateEnemy();
	void JudgeGoNextDay();
	void PlayEnvironment();
};

#pragma endregion

#pragma region Day3
class SceneGame3 : public BaseScene
{
private:
	// エネミーの生成間隔
	int GENERATE_ENEMY_MIN;
	int GENERATE_ENEMY_MAX;
	// 敵ごとの生成確率
	int GENERATE_PER_ENEMY1;
	int GENERATE_PER_ENEMY2;
	int GENERATE_PER_ENEMY3;
	int generateEnemyCount;
	int thisTimeGeneratePerCount;


	int PLAY_SOUND_MIN;
	int PLAY_SOUND_MAX;
	int SOUND_VOLUME_MIN;
	int SOUND_VOLUME_MAX;
	int thisTimePlaySound;
	int SoundVol;

	enum SoundType
	{
		Chaim,
		OpenDoor,
		Rain,
		Pass,
		RingTone,
		Wind,
		Walk,
		Thunder,
		Max,
	};
	Audio* sound[SoundType::Max];
	float  soundVol[SoundType::Max];

	struct LightConstantBuffer
	{
		DirectX::XMFLOAT4	lightColor;
		DirectX::XMFLOAT4	lightDir;
		DirectX::XMFLOAT4	ambientColor;
		DirectX::XMFLOAT4	eyePos;

		PointLight		pointLight[Light::POINT_MAX];
	};

private:
	//				  FPS  sec  minute
	int	TOTAL_NIGHTS = 60 * 60 * 1;
	int nightTime;

public:
	/*----------------------------------------*/
	// Degub variable and function
	DirectX::XMFLOAT3	debugLightPos[Light::POINT_MAX];
	float				debugLightRange[Light::POINT_MAX];
	void UseImGui();
	/*----------------------------------------*/

public:
	SceneGame3() {}
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

	void GenerateEnemy();
	void JudgeGoClear();
	void PlayEnvironment();
};

#pragma endregion

extern SceneGame sceneGame;
extern SceneGame2 sceneGame2;
extern SceneGame3 sceneGame3;