#pragma once

#include <DirectXMath.h>


struct PointLight {
	float index;
	float range;//光の届く範囲
	float type; //有効か無効か
	float dummy;
	DirectX::XMFLOAT4 pos;
	DirectX::XMFLOAT4 color;
};

class Light
{
public:
	enum ConsumptionAmount
	{
		Small,
		Midium,
		Big,
	};
	static int amountState;
private:
	static DirectX::XMFLOAT3 pos;

public:
	static const int POINT_MAX = 32;
	static int		CONSUMPTION_SMALL;
	static int		CONSUMPTION_MIDIUM;
	static int		CONSUMPTION_BIG;

	static DirectX::XMFLOAT4		lightDir;
	static DirectX::XMFLOAT4		dirLightColor;
	static DirectX::XMFLOAT4		ambient;
	static PointLight	pointLight[POINT_MAX];
	static bool			isEnableBattery;

	static long int battery;

public:
	static void Init();
	static void Update();
	static void SetDirLight(DirectX::XMFLOAT3 _dir, DirectX::XMFLOAT3 _color);

	static void SetAmbient(DirectX::XMFLOAT3 _amb);

	static void SetPointLight(int _index, DirectX::XMFLOAT3 _pos, DirectX::XMFLOAT3 _color, float _range);

	// operation light fuction
	static void TurnOffPointLight(int _index);
	static void TurnOnPointLight(int _index);
	static void SwitchPointLight(int _index);

	//Check do light is enable
	static bool JudgeLightEnable(int _index)
	{
		if (pointLight[_index].type == 1)return true;

		return false;
	}

	// setter and getter
	static void SetPos(DirectX::XMFLOAT3 _pos) { pos = _pos; }
	static DirectX::XMFLOAT3 GetPos() { return pos; }
	static int GetBattery() { return battery; }
};