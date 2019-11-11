#include "Light.h"
#include <Windows.h>

DirectX::XMFLOAT4 Light::lightDir(1.0f, 1.0f, 1.0f, 1.0f);
DirectX::XMFLOAT4 Light::dirLightColor(1.0f, 1.0f, 1.0f, 1.0f);
DirectX::XMFLOAT4 Light::ambient(1.0f, 1.0f, 1.0f, 1.0f);
PointLight	Light::pointLight[POINT_MAX];

/*------------------------------------*/
//	初期化
/*------------------------------------*/
void Light::Init()
{
	ZeroMemory(pointLight, sizeof(PointLight) * POINT_MAX);
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
}

/*------------------------------------*/
//	対象のライトをつける
/*------------------------------------*/
void Light::TurnOnPointLight(int _index)
{
	pointLight[_index].type = 1;
}

/*------------------------------------*/
//	対象のライトの状態を反転させる
/*------------------------------------*/
void Light::SwitchPointLight(int _index)
{
	if (pointLight[_index].type == 0)
	{
		pointLight[_index].type = 1;
		return;
	}

	pointLight[_index].type = 0;
}