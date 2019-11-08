#include	"Camera.h"
#include	<framework.h>
#include	"../MyLib/GamePad.h"


Camera::Camera()
{
	pos = DirectX::XMFLOAT3(0.0f, 15.0f, -30.0f);
	target = DirectX::XMFLOAT3(pos.x, pos.y, pos.z + 1.0f);
	angle = 0.0f;

	float fov = DirectX::XMConvertToRadians(30.0f);
	float aspect = static_cast<float>(framework::SCREEN_WIDTH) / static_cast<float>(framework::SCREEN_HEIGHT);
	SetPerspectiveMatrix(fov, aspect, 0.1f, 1000.0f);

	cameraState = CameraState::PlayerCamera;
	state = MoveState::Wait;
	lastState = MoveState::Wait;
	isMove = false;
}

/*------------------------------------*/
//	平行投影
/*------------------------------------*/
DirectX::XMMATRIX	Camera::SetOrthographicMatrix(float width, float height, float zNear, float zFar)
{
	projection = DirectX::XMMatrixOrthographicLH(width, height, zNear, zFar);
	return projection;
}


/*------------------------------------*/
//	透視投影
/*------------------------------------*/
DirectX::XMMATRIX	Camera::SetPerspectiveMatrix(float fov, float aspect, float zNear, float zFar)
{
	projection = DirectX::XMMatrixPerspectiveFovLH(fov, aspect, zNear, zFar);
	return projection;
}


/*------------------------------------*/
//	ビュー行列取得
/*------------------------------------*/
DirectX::XMMATRIX	Camera::GetViewMatrix()
{
	DirectX::XMVECTOR	p = DirectX::XMVectorSet(pos.x, pos.y, pos.z, 1.0f);
	DirectX::XMVECTOR	t = DirectX::XMVectorSet(target.x, target.y, target.z, 1.0f);
	DirectX::XMVECTOR	up = DirectX::XMVectorSet(0.0f/*0.3f*/, 1.0f, 0.0f, 0.0f);

	return DirectX::XMMatrixLookAtLH(p, t, up);
}

/*------------------------------------*/
//	更新関数
/*------------------------------------*/
void Camera::Update()
{
	switch (cameraState)
	{
	case CameraState::PlayerCamera:
		Player();
		if (InputTrigger(XINPUT_BACK)) cameraState = CameraState::WatchCamera;
		break;
	case CameraState::WatchCamera:
		Watch();
		if (InputTrigger(XINPUT_BACK)) cameraState = CameraState::ImGuiCamera;
		break;
	case CameraState::ImGuiCamera:
		UseImGui();
		if (InputTrigger(XINPUT_BACK)) cameraState = CameraState::PlayerCamera;
		break;
	default:
		break;
	}
}

/*------------------------------------*/
//	自由に動くカメラ
/*------------------------------------*/
void Camera::Watch()
{
	if (InputState(XINPUT_DPAD_RIGHT))
	{
		pos.x += 1.0f;
	}
	else if (InputState(XINPUT_DPAD_LEFT))
	{
		pos.x -= 1.0f;
	}

	if (InputState(XINPUT_DPAD_UP))
	{
		pos.z += 1.0f;
	}
	else if (InputState(XINPUT_DPAD_DOWN))
	{
		pos.z -= 1.0f;
	}

	if (InputState(XINPUT_A))
	{
		pos.y -= 1.0f;
	}
	else if (InputState(XINPUT_Y))
	{
		pos.y += 1.0f;
	}

	target = pos;

	target.z += 1.0f;
}

/*------------------------------------*/
//	プレイヤーカメラ
/*------------------------------------*/
void Camera::Player()
{
	if (InputState(XINPUT_DPAD_RIGHT) && state != MoveState::Shift_Left)
	{
		lastState = state;
		state = MoveState::Shift_Right;
		isMove = true;
	}
	else if (InputState(XINPUT_DPAD_LEFT) && state != MoveState::Shift_Right)
	{
		lastState = state;
		state = MoveState::Shift_Left;
		isMove = true;
	}
	else if(state != MoveState::Wait)
	{
		lastState = state;
		state = MoveState::Wait;
	}

	if (!isMove)return;

	switch (state)
	{
	case Camera::Wait:
		if (lastState == MoveState::Shift_Left)
		{
			pos.x += MOVE_SPEED;
			if (pos.x >= 0.0f)
			{
				pos.x = 0.0f;
				isMove = false;
			}
		}
		else if (lastState == MoveState::Shift_Right)
		{
			pos.x -= MOVE_SPEED;
			if (pos.x <= 0.0f)
			{
				pos.x = 0.0f;
				isMove = false;
			}
		}

		break;
	case Camera::Shift_Left:
		pos.x -= MOVE_SPEED;
		if (pos.x <= -40.0f)
		{
			pos.x = -40.0f;
		}
		break;
	case Camera::Shift_Right:
		pos.x += MOVE_SPEED;
		if (pos.x >= 40.0f)
		{
			pos.x = 40.0f;
		}
		break;
	default:
		break;
	}

//	pos.z = -37.0f;
	pos.z = -30.0f;
	pos.y = 15.0f;
	target = pos;

	target.z += 1.0f;

}

/*------------------------------------*/
//	ImGuiを使って値を動かすカメラ
/*------------------------------------*/
void Camera::UseImGui()
{
	ImGui::Begin("ざひょー");

	ImGui::SliderFloat("pos.x", &pos.x, -200.0f, 200.0f);
	ImGui::SliderFloat("pos.y", &pos.y, -200.0f, 200.0f);
	ImGui::SliderFloat("pos.z", &pos.z, -200.0f, 200.0f);

	ImGui::End();
}

/*------------------------------------*/
//	右に倒している時の動き
/*------------------------------------*/


/*------------------------------------*/
//	左に倒している時の動き
/*------------------------------------*/


/*------------------------------------*/
//	倒していない時の動き
/*------------------------------------*/
