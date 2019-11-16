#include	"Camera.h"
#include	<framework.h>
#include	"../MyLib/GamePad.h"
#include	"Light.h"
#include	"Stage.h"


Camera::Camera()
{
	Init();
}

void Camera::Init()
{
	pos = DirectX::XMFLOAT3(0.0f, 23.0f, -30.0f);
	target = DirectX::XMFLOAT3(pos.x, pos.y, pos.z + 1.0f);
	upVector = DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f);
	angle = 0.0f;
	time = 0;

	float fov = DirectX::XMConvertToRadians(30.0f);
	float aspect = static_cast<float>(framework::SCREEN_WIDTH) / static_cast<float>(framework::SCREEN_HEIGHT);
	SetPerspectiveMatrix(fov, aspect, 0.1f, 1000.0f);

	cameraState = CameraState::PlayerCamera;
	state = MoveState::Wait;
	lastState = MoveState::Wait;
	isMove = false;
	canPushSwitch = false;
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
	DirectX::XMVECTOR	up = DirectX::XMVectorSet(upVector.x, upVector.y, upVector.z, 0.0f);

	return DirectX::XMMatrixLookAtLH(p, t, up);
}

/*------------------------------------*/
//	更新関数
/*------------------------------------*/
void Camera::Update(bool _isNotOperation)
{
	switch (cameraState)
	{
	case CameraState::PlayerCamera:
		Player(_isNotOperation);
#ifdef USE_IMGUI
		UseImGui();
#endif
//		if (InputTrigger(XINPUT_BACK)) cameraState = CameraState::WatchCamera;
		break;
	case CameraState::WatchCamera:
		Watch(); 
#ifdef USE_IMGUI
		UseImGui();
#endif
//		if (InputTrigger(XINPUT_BACK)) cameraState = CameraState::PlayerCamera;
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
void Camera::Player(bool _isNotOperation)
{
	if (InputState(XINPUT_DPAD_RIGHT) && !_isNotOperation)
	{
		if (!isMove)
		{
			lastState = state;
			state = MoveState::Shift_Right;
			isMove = true;
		}
	}
	else if (InputState(XINPUT_DPAD_LEFT) && !_isNotOperation)
	{
		if (!isMove)
		{
			lastState = state;
			state = MoveState::Shift_Left;
			isMove = true;
		}
	}
	else if(state != MoveState::Wait)
	{
		if (state != MoveState::Wait)
		{
			lastState = state;
			state = MoveState::Wait;
		}
	}

//	if (InputTrigger(XINPUT_B) && canPushSwitch && Light::isEnableBattery)
//	{
//		switch (state)
//		{
//		case Camera::Shift_Left:
//			Light::SwitchPointLight(0);
//			break;
//		case Camera::Shift_Right:
//			Light::SwitchPointLight(1);
//			break;
//		default:
//			break;
//		}
//	}

	if (!isMove)return;

	switch (state)
	{
	case Camera::Wait:
		MoveWait();
		break;
	case Camera::Shift_Left:
		MoveLeft();
		break;
	case Camera::Shift_Right:
		MoveRight();
		break;
	default:
		break;
	}

//	pos.z = -37.0f;
//	pos.z = -30.0f;
//	pos.y = 15.0f;
	target = pos;

	target.z += 1.0f;

}

/*------------------------------------*/
//	ImGuiを使って値を動かすカメラ
/*------------------------------------*/
#ifdef USE_IMGUI
void Camera::UseImGui()
{
	ImGui::Begin(" Camera ");

	ImGui::SliderFloat("pos.x", &pos.x, -200.0f, 200.0f);
	ImGui::SliderFloat("pos.y", &pos.y, -200.0f, 200.0f);
	ImGui::SliderFloat("pos.z", &pos.z, -200.0f, 200.0f);
	ImGui::NewLine();
	ImGui::SliderFloat(" MOVE_SPEED(Frame)", &MOVE_SPEED, 0.1f, 5.0f);
	ImGui::InputInt(" PEEKING_COUNT(Frame)", &PEEKING_COUNT);

	ImGui::End();
}
#endif

/*------------------------------------*/
//	右に倒している時の動き
/*------------------------------------*/
// (18, 15, 27)
void Camera::MoveRight()
{
	Gana::Vector3 dirVec;
	DirectX::XMFLOAT3 targetPos;
	switch (walkState)
	{
	case Camera::Walking:
		// 移動ベクトル
		targetPos = DirectX::XMFLOAT3(18.0f, 23.0f, 27.0f);
		dirVec.x = targetPos.x - pos.x;
		dirVec.y = targetPos.y - pos.y;
		dirVec.z = targetPos.z - pos.z;
		dirVec.Normalize();

		pos.x = pos.x + dirVec.x * MOVE_SPEED;
		pos.y = pos.y + dirVec.y * MOVE_SPEED;
		pos.z = pos.z + dirVec.z * MOVE_SPEED;

		if (pos.x >= 18.0f)
		{
			walkState = Peeking;
			time = 0;
			Light::TurnOnPointLight(1);
		}
		break;
	case Camera::Peeking:
		if (time++ < PEEKING_COUNT)
		{
			upVector.x = 0.5f * time / PEEKING_COUNT;
			pos.x += 0.1 * 30 / PEEKING_COUNT;
			// ドア傾ける
			Stage::Get().rightDoorAngle += DirectX::XMConvertToRadians(1.0f) * 30 / PEEKING_COUNT;
		}
		if (time >= PEEKING_COUNT)
		{
			time = PEEKING_COUNT;
			canPushSwitch = true;
		}

		break;
	default:
		break;
	}
}

/*------------------------------------*/
//	左に倒している時の動き
/*------------------------------------*/
// (-18, 15, 27)
void Camera::MoveLeft()
{
	Gana::Vector3 dirVec;
	DirectX::XMFLOAT3 targetPos;
	switch (walkState)
	{
	case Camera::Walking:
		// 移動ベクトル
		targetPos = DirectX::XMFLOAT3(-18.0f, 23.0f, 27.0f);
		dirVec.x = targetPos.x - pos.x;
		dirVec.y = targetPos.y - pos.y;
		dirVec.z = targetPos.z - pos.z;
		dirVec.Normalize();

		pos.x = pos.x + dirVec.x * MOVE_SPEED;
		pos.y = pos.y + dirVec.y * MOVE_SPEED;
		pos.z = pos.z + dirVec.z * MOVE_SPEED;

		if (pos.x <= -18.0f)
		{
			walkState = Peeking;
			time = 0;
			Light::TurnOnPointLight(0);
		}
		break;
	case Camera::Peeking:
		if (time++ < PEEKING_COUNT)
		{
			upVector.x = -0.5f * time / PEEKING_COUNT;
			pos.x -= 0.1 * 30 / PEEKING_COUNT;
			Stage::Get().leftDoorAngle -= DirectX::XMConvertToRadians(1.0f) * 30 / PEEKING_COUNT;
		}
		if (time >= PEEKING_COUNT)
		{
			time = PEEKING_COUNT;
			canPushSwitch = true;
		}

		break;
	default:
		break;
	}

}

/*------------------------------------*/
//	倒していない時の動き
/*------------------------------------*/
void Camera::MoveWait()
{
	if (lastState == MoveState::Shift_Left)
	{
		Gana::Vector3 dirVec;
		DirectX::XMFLOAT3 targetPos;

		switch (walkState)
		{
		case Camera::Peeking:
			if (time-- <= PEEKING_COUNT)
			{
				canPushSwitch = false;
				upVector.x = -0.5f * time / PEEKING_COUNT;
				pos.x += 0.1 * 30 / PEEKING_COUNT;
				Stage::Get().leftDoorAngle += DirectX::XMConvertToRadians(1.0f) * 30 / PEEKING_COUNT;
			}

			if (time == 0)
			{
				walkState = Camera::Walking;
				Light::TurnOffPointLight(0);
			}

			break;
		case Camera::Walking:
			// 移動ベクトル
			targetPos = DirectX::XMFLOAT3(0.0f, 23.0f, -30.0f);
			dirVec.x = targetPos.x - pos.x;
			dirVec.y = targetPos.y - pos.y;
			dirVec.z = targetPos.z - pos.z;
			dirVec.Normalize();

			pos.x = pos.x + dirVec.x * MOVE_SPEED;
			pos.y = pos.y + dirVec.y * MOVE_SPEED;
			pos.z = pos.z + dirVec.z * MOVE_SPEED;

			if (pos.x >= 0.0f)
			{
				isMove = false;
				pos = targetPos;
			}

			break;
		default:
			break;
		}
	}
	else if (lastState == MoveState::Shift_Right)
	{
		Gana::Vector3 dirVec;
		DirectX::XMFLOAT3 targetPos;

		switch (walkState)
		{
		case Camera::Peeking:
			if (time-- <= PEEKING_COUNT)
			{
				canPushSwitch = false;
				upVector.x = 0.5f * time / PEEKING_COUNT;
				pos.x -= 0.1 * 30 / PEEKING_COUNT;
				Stage::Get().rightDoorAngle -= DirectX::XMConvertToRadians(1.0f) * 30 / PEEKING_COUNT;
			}

			if (time == 0)
			{
				walkState = Camera::Walking;
				Light::TurnOffPointLight(1);
			}

			break;
		case Camera::Walking:
			// 移動ベクトル
			targetPos = DirectX::XMFLOAT3(0.0f, 23.0f, -30.0f);
			dirVec.x = targetPos.x - pos.x;
			dirVec.y = targetPos.y - pos.y;
			dirVec.z = targetPos.z - pos.z;
			dirVec.Normalize();

			pos.x = pos.x + dirVec.x * MOVE_SPEED;
			pos.y = pos.y + dirVec.y * MOVE_SPEED;
			pos.z = pos.z + dirVec.z * MOVE_SPEED;

			if (pos.x <= 0.0f)
			{
				isMove = false;
				pos = targetPos;
			}

			break;
		default:
			break;
		}
	}

}