#pragma once

#include <DirectXMath.h>
#include <Vector.h>

#include "framework.h"

class Camera
{
private:
	float	MOVE_SPEED = 4.0f;
	int		PEEKING_COUNT = 18;
	enum CameraState
	{
		PlayerCamera,
		WatchCamera,
	};
	enum WalkState
	{
		Walking,
		Peeking,
	};

public:
	enum MoveState
	{
		Wait,
		Shift_Left,
		Shift_Right,
	};

private:
	MoveState	state;
	MoveState	lastState;
	CameraState	cameraState;
	WalkState	walkState;

	bool	isMove;
	bool	canPushSwitch;
	int		time;

//public:
	DirectX::XMFLOAT3	pos;		// 座標
	DirectX::XMFLOAT3	target;		// 注視点
	DirectX::XMFLOAT3	upVector;	// 上方向ベクトル
	DirectX::XMMATRIX	projection;	// 投影行列
	float				angle;		// カメラの角度

public:
	~Camera(){}
	void		Init();
	void		Update(bool _isNotOperation = false);

private:
	Camera();
	void		Watch();
	void		Player(bool _isNotOperation = false);

	void UseImGui();

	void MoveRight();
	void MoveLeft();
	void MoveWait();


public:
	DirectX::XMMATRIX	SetOrthographicMatrix( float _width, float _height, float _zNear, float _zFar );
	DirectX::XMMATRIX	SetPerspectiveMatrix( float _fov, float _aspect, float _zNear, float _zFar );
	DirectX::XMMATRIX	GetViewMatrix();
	DirectX::XMMATRIX	GetProjectionMatrix() { return projection; }

	// getter and setter
	DirectX::XMFLOAT3	GetPos() { return pos; }
	bool				GetCanPushSwitch() { return canPushSwitch; }
	MoveState			GetMoveState() { return state; }

	static Camera& Get()
	{
		static Camera instance;
		return instance;
	}
};