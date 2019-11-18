#pragma once

#include <DirectXMath.h>
#include <Vector.h>

#include "framework.h"
#include "Sound.h"

class Camera
{
private:
	float	MOVE_SPEED = 4.0f;
	int		PEEKING_COUNT = 18;

	enum SoundType
	{
		Discover,
		Max,
	};
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
	Audio*		sound[SoundType::Max];

	bool	isMove;
	bool	canPushSwitch;
	int		time;

//public:
	DirectX::XMFLOAT3	pos;		// ���W
	DirectX::XMFLOAT3	target;		// �����_
	DirectX::XMFLOAT3	upVector;	// ������x�N�g��
	DirectX::XMMATRIX	projection;	// ���e�s��
	float				angle;		// �J�����̊p�x

public:
	~Camera()
	{
		for (auto& it : sound)
		{
			delete it;
			it = nullptr;
		}
	}
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

	bool CheckLookEnemy(bool _isLookRight);


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