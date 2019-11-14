#include "Light.h"

#include <Windows.h>
#include <framework.h>

DirectX::XMFLOAT4 Light::lightDir(1.0f, 1.0f, 1.0f, 1.0f);
DirectX::XMFLOAT4 Light::dirLightColor(1.0f, 1.0f, 1.0f, 1.0f);
DirectX::XMFLOAT4 Light::ambient(1.0f, 1.0f, 1.0f, 1.0f);
PointLight	Light::pointLight[POINT_MAX];
int Light::CONSUMPTION_SMALL;
int Light::CONSUMPTION_MIDIUM;
int Light::CONSUMPTION_BIG;
long int Light::battery;
int Light::amountState;
static Light::ConsumptionAmount amountState;
bool Light::isEnableBattery;

/*------------------------------------*/
//	初期化
/*------------------------------------*/
void Light::Init()
{
	ZeroMemory(pointLight, sizeof(PointLight) * POINT_MAX);
	//		   %    FPS  seconds
	battery = 100 * 60 * 5;
	isEnableBattery = true;
	CONSUMPTION_SMALL = 1;
	CONSUMPTION_MIDIUM = 2;
	CONSUMPTION_BIG = 3;

	amountState = ConsumptionAmount::Small;
}

void Light::Update()
{
#ifdef USE_IMGUI
	ImGui::Begin("light");
	ImGui::InputInt("SMALL", &CONSUMPTION_SMALL);
	ImGui::InputInt("MIDIUM", &CONSUMPTION_MIDIUM);
	ImGui::InputInt("BIG", &CONSUMPTION_BIG);
	ImGui::NewLine();
	ImGui::Text("Battery : %d", battery);
	ImGui:: End();
#endif

	if (!isEnableBattery)return;
	switch (amountState)
	{
	case 0:	// small
		battery -= static_cast<unsigned int>(CONSUMPTION_SMALL);
		break;
	case 1: // midium
		battery -= static_cast<unsigned int>(CONSUMPTION_MIDIUM);
		break;
	case 2: // big
		battery -= static_cast<unsigned int>(CONSUMPTION_BIG);
		break;
	default:
		break;
	}

	if (battery <= 0)
	{
		battery = 0;
		isEnableBattery = false;
		TurnOffPointLight(0);
		TurnOffPointLight(1);
	}
}

/*------------------------------------*/
//	ライト方向ベクトルの設定
/*------------------------------------*/
void Light::SetDirLight(DirectX::XMFLOAT3 _dir, DirectX::XMFLOAT3 _color)
{
	float d = sqrtf(_dir.x * _dir.x + _dir.y * _dir.y + _dir.z * _dir.z);

	if (d > 0) { _dir.x /= d, _dir.y /= d, _dir.z /= d; }
	lightDir = DirectX::XMFLOAT4(_dir.x, _dir.y, _dir.z, 0);
	dirLightColor = DirectX::XMFLOAT4(_color.x, _color.y, _color.z, 1);
}

/*------------------------------------*/
//	環境光の設定
/*------------------------------------*/
void Light::SetAmbient(DirectX::XMFLOAT3 _amb)
{
	ambient = { _amb.x, _amb.y,_amb.z,0 };
}

/*------------------------------------*/
//	ポイントライトを設定する
/*------------------------------------*/
void Light::SetPointLight(int _index, DirectX::XMFLOAT3 _pos, DirectX::XMFLOAT3 _color, float _range)
{
	if (_index < 0)return;
	if (_index >= POINT_MAX)return;

	PointLight& it = pointLight[_index];
	it.index = (float)_index;
	it.range = _range;
	it.type = 1.0f;
	it.dummy = 0.0f;
	it.pos = DirectX::XMFLOAT4(_pos.x, _pos.y, _pos.z, 0);
	it.color = DirectX::XMFLOAT4(_color.x, _color.y, _color.z, 0);
}

/*------------------------------------*/
//	対象のライトを消す
/*------------------------------------*/
void Light::TurnOffPointLight(int _index)
{
	pointLight[_index].type = 0;
	amountState--;
}

/*------------------------------------*/
//	対象のライトをつける
/*------------------------------------*/
void Light::TurnOnPointLight(int _index)
{
	pointLight[_index].type = 1;
	amountState++;
}

/*------------------------------------*/
//	対象のライトの状態を反転させる
/*------------------------------------*/
void Light::SwitchPointLight(int _index)
{
	if (pointLight[_index].type == 0)
	{
		pointLight[_index].type = 1;
		amountState++;
		return;
	}

	pointLight[_index].type = 0;
	amountState--;

}