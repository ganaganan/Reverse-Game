#pragma once

#include <DirectXMath.h>
#include "framework.h"

class Camera
{
private:
	static constexpr float MOVE_SPEED = 3.0f;
	enum MoveState
	{
		Wait,
		Shift_Left,
		Shift_Right,
	};
	MoveState	state;
	MoveState	lastState;

	enum CameraState
	{
		PlayerCamera,
		WatchCamera,
		ImGuiCamera,
	};
	CameraState	cameraState;

	bool	isMove;
	int		time;

//public:
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
	void		Player();

#ifdef USE_IMGUI
	void UseImGui();
#endif

	void MoveRight();
	void MoveLeft();
	void MoveWait();


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