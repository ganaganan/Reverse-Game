#pragma once

#include <DirectXMath.h>
#include "framework.h"

class Camera
{
private:
	static constexpr float MOVE_SPEED = 3.0f;
	enum State
	{
		Wait,
		Shift_Left,
		Shift_Right,
	};
	State	state;
	State	lastState;
	bool	isMove;
	int		time;

//public:
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
	void		Player();

#ifdef USE_IMGUI
	void UseImGui();
#endif


public:
	DirectX::XMMATRIX	SetOrthographicMatrix( float _width, float _height, float _zNear, float _zFar );
	DirectX::XMMATRIX	SetPerspectiveMatrix( float _fov, float _aspect, float _zNear, float _zFar );
	DirectX::XMMATRIX	GetViewMatrix();
	DirectX::XMMATRIX	GetProjectionMatrix() { return projection; }

	DirectX::XMFLOAT3	GetPos() { return pos; }

	static Camera& Get()
	{
		static Camera instance;
		return instance;
	}
};