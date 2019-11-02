#pragma once

#include <DirectXMath.h>

class Camera
{
public:
	DirectX::XMFLOAT3	pos;		// 座標
	DirectX::XMFLOAT3	target;		// 注視点
	DirectX::XMMATRIX	projection;	// 投影行列
	float				angle;		// カメラの角度

public:
	~Camera(){}
	void		Update();

private:
	Camera();
	void		Watch();

public:
	DirectX::XMMATRIX	SetOrthographicMatrix(float _width, float _height, float _zNear, float _zFar);
	DirectX::XMMATRIX	SetPerspectiveMatrix(float _fov, float _aspect, float _zNear, float _zFar);
	DirectX::XMMATRIX	GetViewMatrix();
	DirectX::XMMATRIX	GetProjectionMatrix() { return projection; }


	static Camera& Get()
	{
		static Camera instance;
		return instance;
	}
};

