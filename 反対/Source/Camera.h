#pragma once

#include <DirectXMath.h>

class Camera
{
public:
	DirectX::XMFLOAT3	pos;		// ���W
	DirectX::XMFLOAT3	target;		// �����_
	DirectX::XMMATRIX	projection;	// ���e�s��
	float				angle;		// �J�����̊p�x

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

