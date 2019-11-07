#include	"Camera.h"
#include	<framework.h>
#include	"../MyLib/GamePad.h"
/*------------------------------------*/
//	Cameraクラス
/*------------------------------------*/
Camera::Camera()
{
	pos = DirectX::XMFLOAT3(0.0f, 0.0f, -5.0f);
	target = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
	angle = 0.0f;

	float fov = DirectX::XMConvertToRadians(30.0f);
	float aspect = static_cast<float>(framework::SCREEN_WIDTH) / static_cast<float>(framework::SCREEN_HEIGHT);
	SetPerspectiveMatrix(fov, aspect, 0.1f, 1000.0f);

	state = State::Wait;
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
	DirectX::XMVECTOR	up = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	return DirectX::XMMatrixLookAtLH(p, t, up);
}

void Camera::Update()
{
//	Watch();
	Player();
//	UseImGui();
}

void Camera::Watch()
{
	if (InputState(XINPUT_DPAD_RIGHT))
	{
		pos.x += 0.1f;
	}
	else if (InputState(XINPUT_DPAD_LEFT))
	{
		pos.x -= 0.1f;
	}

	if (InputState(XINPUT_DPAD_UP))
	{
		pos.z += 0.1f;
	}
	else if (InputState(XINPUT_DPAD_DOWN))
	{
		pos.z -= 0.1f;
	}

	if (InputState(XINPUT_A))
	{
		pos.y -= 0.1f;
	}
	else if (InputState(XINPUT_Y))
	{
		pos.y += 0.1f;
	}

	target = pos;

	target.z += 1.0f;
}

void Camera::Player()
{
	if (InputState(XINPUT_DPAD_RIGHT) && state != State::Shift_Left)
	{
		lastState = state;
		state = State::Shift_Right;
		isMove = true;
	}
	else if (InputState(XINPUT_DPAD_LEFT) && state != State::Shift_Right)
	{
		lastState = state;
		state = State::Shift_Left;
		isMove = true;
	}
	else if(state != State::Wait)
	{
		lastState = state;
		state = State::Wait;
	}

	if (!isMove)return;

	switch (state)
	{
	case Camera::Wait:
		if (lastState == State::Shift_Left)
		{
			pos.x += MOVE_SPEED;
			if (pos.x >= 0.0f)
			{
				pos.x = 0.0f;
				isMove = false;
			}
		}
		else if (lastState == State::Shift_Right)
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

void Camera::UseImGui()
{
	ImGui::Begin("ざひょー");

	ImGui::SliderFloat("pos.x", &pos.x, -200.0f, 200.0f);
	ImGui::SliderFloat("pos.y", &pos.y, -200.0f, 200.0f);
	ImGui::SliderFloat("pos.z", &pos.z, -200.0f, 200.0f);

	ImGui::End();
}