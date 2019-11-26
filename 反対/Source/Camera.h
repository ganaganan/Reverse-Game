#pragma once

#include <DirectXMath.h>
#include <Vector.h>

#include "framework.h"
#include "Sound.h"
#include "SceneTutorial.h"

class Camera
{
private:
	float	MOVE_SPEED = 4.0f;
	int		PEEKING_COUNT = 18;
public:
	enum SoundType
	{
		Discover,
		Switch,
		ShutDown,//TODO:Light�ɂ��ꂽ��
		Max,
	};
private:
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
	DirectX::XMFLOAT3	pos;		// ���W
	DirectX::XMFLOAT3	target;		// �����_
	DirectX::XMFLOAT3	upVector;	// ������x�N�g��
	DirectX::XMMATRIX	projection;	// ���e�s��
	float				angle;		// �J�����̊p�x
public:
	Audio* sound[SoundType::Max];

public:
	~Camera()
	{
		Uninit();
	}
	void		Init();
	void		Uninit();
	void		Update(bool _isNotOperation = false);
	void		UpdateTutorial();
private:
	Camera();
	void		Watch();
	void		Player(bool _isNotOperation = false);

	void UseImGui();

	void MoveRight(bool _isTutorial = false);
	void MoveLeft(bool _isTutorial = false);
	void MoveWait(bool _isTutorial = false);

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