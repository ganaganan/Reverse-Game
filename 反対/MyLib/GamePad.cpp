#include "GamePad.h"
#include <Xinput.h>
#include "framework.h"
#include <dinput.h>
//#include "CWindow.h"

#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>
#include <commdlg.h>
#include <basetsd.h>
#include <objbase.h>
#ifdef USE_DIRECTX_SDK
#include <C:\Program Files (x86)\Microsoft DirectX SDK (June 2010)\include\xinput.h>
#pragma comment(lib,"xinput.lib")
#elif (_WIN32_WINNT >= 0x0602 /*_WIN32_WINNT_WIN8*/)
#include <XInput.h>
#pragma comment(lib,"xinput.lib")
#else
#include <XInput.h>
#pragma comment(lib,"xinput9_1_0.lib")
#endif

CONTROLLER_STATE controllers[MAX_CONTROLLERS];
int triggerCount[TRG_NUM_MAX][BUTTONS::DPAD_END];
std::map<int, int> checkButton;
//LPDIRECTINPUT8			lpDI = NULL;		// IDirectInput8
//LPDIRECTINPUTDEVICE8	lpJoystick = NULL;	// ジョイスティックデバイス
DInput					dInput;				// DInput


//for文で回すためだけのテーブル
int config_tbl[] = {
	XINPUT_DPAD_UP,
	XINPUT_DPAD_DOWN,
	XINPUT_DPAD_LEFT,
	XINPUT_DPAD_RIGHT,
	XINPUT_START,
	XINPUT_BACK,
	XINPUT_LEFT_THUMB,
	XINPUT_RIGHT_THUMB,
	XINPUT_LEFT_SHOULDER,
	XINPUT_RIGHT_SHOULDER,
	XINPUT_A,
	XINPUT_B,
	XINPUT_X,
	XINPUT_Y,
};


HRESULT		InitControllers()
{
	ZeroMemory(controllers, sizeof(CONTROLLER_STATE) * MAX_CONTROLLERS);

	for (int i = 0; i < TRG_NUM_MAX; i++)
	{
		for (auto& it : triggerCount[i])
		{
			it = 0;
		}
	}

	for (int i = 0; i < BUTTONS::DPAD_END; i++)
	{
		checkButton.insert(std::make_pair(config_tbl[i], i));
	}

	// DirectInput
	dInput.InitDirectInput();

	return S_OK;
}

HRESULT		UpdateControllerState()
{
	DWORD dwResult;
	for (DWORD i = 0; i < MAX_CONTROLLERS; i++)
	{
		dwResult = XInputGetState(i, &controllers[i].state);

		if (dwResult == ERROR_SUCCESS)
			controllers[i].bConnected = true;
		else
			controllers[i].bConnected = false;
	}

	ChackInputState();

	// DirectInput
	dInput.UpdateDirectInput();

	return S_OK;
}

void ChackInputState()
{
	for (int s = 0; s < TRG_NUM_MAX; s++)
	{
		for (int i = 0; i < BUTTONS::DPAD_END; i++)
		{
			if (InputState(config_tbl[i], s))
			{
				triggerCount[s][i]++;
			}
			else
			{
				triggerCount[s][i] = 0;
			}
		}
	}

	//dInput
	for (int i = 0; i < 4; i++)
	{
		if (DPadInputState(i))
		{
			dInput.dPadTriggerCount[i]++;
		}
		else
		{
			dInput.dPadTriggerCount[i] = 0;
		}
	}
}

void	RenderFrame()
{

	for (DWORD i = 0; i < MAX_CONTROLLERS; i++)
	{
		if (!controllers[i].bConnected)continue;
		WORD wButtons = controllers[i].state.Gamepad.wButtons;
		
		//Draw XInput Buttons
		{
			//																						  String				x		y		sx	sy
					//if (wButtons & XINPUT_A)				font->textout(FRAMEWORK->GetDeviceContext(), "A",				100,	0,		16, 16);
					//if (wButtons & XINPUT_B)				font->textout(FRAMEWORK->GetDeviceContext(), "B",				100,	20,		16, 16);
					//if (wButtons & XINPUT_X)				font->textout(FRAMEWORK->GetDeviceContext(), "X",				100,	40,		16, 16);
					//if (wButtons & XINPUT_Y)				font->textout(FRAMEWORK->GetDeviceContext(), "Y",				100,	60,		16, 16);
					//if (wButtons & XINPUT_START)			font->textout(FRAMEWORK->GetDeviceContext(), "START",			100,	80,		16, 16);
					//if (wButtons & XINPUT_BACK)				font->textout(FRAMEWORK->GetDeviceContext(), "BACK",			100,	100,	16, 16);
					//if (wButtons & XINPUT_LEFT_THUMB)		font->textout(FRAMEWORK->GetDeviceContext(), "LEFT_THUMB",		100,	120,	16, 16);
					//if (wButtons & XINPUT_RIGHT_THUMB)		font->textout(FRAMEWORK->GetDeviceContext(), "RIGHT_THUMB",		100,	140,	16, 16);
					//if (wButtons & XINPUT_LEFT_SHOULDER)	font->textout(FRAMEWORK->GetDeviceContext(), "LEFT_SHOULDER",	100,	160,	16, 16);
					//if (wButtons & XINPUT_RIGHT_SHOULDER)	font->textout(FRAMEWORK->GetDeviceContext(), "RIGHT_SHOULDER",	100,	180,	16, 16);
					//if (wButtons & XINPUT_DPAD_DOWN)		font->textout(FRAMEWORK->GetDeviceContext(), "DOWN",			100,	200,	16, 16);
					//if (wButtons & XINPUT_DPAD_LEFT)		font->textout(FRAMEWORK->GetDeviceContext(), "LEFT",			100,	220,	16, 16);
					//if (wButtons & XINPUT_DPAD_RIGHT)		font->textout(FRAMEWORK->GetDeviceContext(), "RIGHT",			100,	240,	16, 16);
					//if (wButtons & XINPUT_DPAD_UP)			font->textout(FRAMEWORK->GetDeviceContext(), "UP",				100,	260,	16, 16);
		}
//		if (InputState (XINPUT_A))				font->textout(FRAMEWORK->GetDeviceContext(), "A",				100,	0,		16, 16);
//		if (InputState (XINPUT_B))				font->textout(FRAMEWORK->GetDeviceContext(), "B",				100,	20,		16, 16);
//		if (InputState (XINPUT_X))				font->textout(FRAMEWORK->GetDeviceContext(), "X",				100,	40,		16, 16);
//		if (InputState (XINPUT_Y))				font->textout(FRAMEWORK->GetDeviceContext(), "Y",				100,	60,		16, 16);
//		if (InputState (XINPUT_START))			font->textout(FRAMEWORK->GetDeviceContext(), "START",			100,	80,		16, 16);
//		if (InputState (XINPUT_BACK))			font->textout(FRAMEWORK->GetDeviceContext(), "BACK",			100,	100,	16, 16);
//		if (InputState (XINPUT_LEFT_THUMB))		font->textout(FRAMEWORK->GetDeviceContext(), "LEFT_THUMB",		100,	120,	16, 16);
//		if (InputState (XINPUT_RIGHT_THUMB))	font->textout(FRAMEWORK->GetDeviceContext(), "RIGHT_THUMB",		100,	140,	16, 16);
//		if (InputState (XINPUT_LEFT_SHOULDER))	font->textout(FRAMEWORK->GetDeviceContext(), "LEFT_SHOULDER",	100,	160,	16, 16);
//		if (InputState (XINPUT_RIGHT_SHOULDER))	font->textout(FRAMEWORK->GetDeviceContext(), "RIGHT_SHOULDER",	100,	180,	16, 16);
//		if (InputState (XINPUT_DPAD_DOWN))		font->textout(FRAMEWORK->GetDeviceContext(), "DOWN",			100,	200,	16, 16);
//		if (InputState (XINPUT_DPAD_LEFT))		font->textout(FRAMEWORK->GetDeviceContext(), "LEFT",			100,	220,	16, 16);
//		if (InputState (XINPUT_DPAD_RIGHT))		font->textout(FRAMEWORK->GetDeviceContext(), "RIGHT",			100,	240,	16, 16);
//		if (InputState (XINPUT_DPAD_UP))		font->textout(FRAMEWORK->GetDeviceContext(), "UP",				100,	260,	16, 16);


	}
}

bool	InputTrigger(int _input, int _padNum)
{
	if (_input <= 0)return false;

	if (!controllers[_padNum].bConnected)return false;

	if (controllers[_padNum].state.Gamepad.wButtons & _input)
	{
		if (triggerCount[_padNum][checkButton.at(_input)] == 1)
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	return false;
}

bool	InputState(int _input, int _padNum)
{
	if (_input <= 0)return false;

//	for (DWORD i = 0; i < MAX_CONTROLLERS; i++)
//	{
//		if (!controllers[i].bConnected)continue;
//
//		if (controllers[i].state.Gamepad.wButtons & _input)
//		{
//			return true;
//		}
//	}
//	return false;
	if (!controllers[_padNum].bConnected)return false;

	if (controllers[_padNum].state.Gamepad.wButtons & _input)
	{
		return true;
	}
	else
	{
		return false;
	}

}



//------------------------------------------------------------
// DirectInput関係
//------------------------------------------------------------
// コールバック関数

BOOL PASCAL EnumJoyDeviceProc(LPCDIDEVICEINSTANCE lpddi, LPVOID pvRef)
{
	LPDIRECTINPUTDEVICE8	_lpJoystick = NULL;

	// ジョイスティックデバイスの作成
	HRESULT hr = dInput.lpDI->CreateDevice(lpddi->guidInstance, &_lpJoystick, NULL);
	assert(SUCCEEDED(hr) && "GamePad.cpp line : 204");

	// 入力データ形式のセット
	hr = _lpJoystick->SetDataFormat( &c_dfDIJoystick );
	assert(SUCCEEDED(hr) && "GamePad.cpp line : 209");

	// 排他制御のセット
	hr = _lpJoystick->SetCooperativeLevel(dInput.hwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY);
	assert(SUCCEEDED(hr) && "GamePad.cpp line : 221");

	// 入力範囲のセット
	DIPROPRANGE	diprg;
	diprg.diph.dwSize = sizeof(diprg);
	diprg.diph.dwHeaderSize = sizeof(diprg.diph);
	diprg.diph.dwHow = DIPH_BYOFFSET;
	diprg.lMax = 1000;
	diprg.lMin = -1000;

	// X軸
	diprg.diph.dwObj = DIJOFS_X;
	_lpJoystick->SetProperty(DIPROP_RANGE, &diprg.diph);

	// Y軸
	diprg.diph.dwObj = DIJOFS_Y;
	_lpJoystick->SetProperty(DIPROP_RANGE, &diprg.diph);

	// Z軸
	diprg.diph.dwObj = DIJOFS_Z;
	_lpJoystick->SetProperty(DIPROP_RANGE, &diprg.diph);

	// RX軸
	diprg.diph.dwObj = DIJOFS_RX;
	_lpJoystick->SetProperty(DIPROP_RANGE, &diprg.diph);

	// RY軸
	diprg.diph.dwObj = DIJOFS_RY;
	_lpJoystick->SetProperty(DIPROP_RANGE, &diprg.diph);

	// RZ軸
	diprg.diph.dwObj = DIJOFS_RZ;
	_lpJoystick->SetProperty(DIPROP_RANGE, &diprg.diph);

	// 起動準備完了
	_lpJoystick->Poll();

	// 構築完了なら
	char tmp[260];
	WideCharToMultiByte(CP_ACP, 0, lpddi->tszInstanceName, -1, tmp, sizeof(tmp), NULL, NULL);
	WideCharToMultiByte(CP_ACP, 0, lpddi->tszProductName, -1, tmp, sizeof(tmp), NULL, NULL);

	dInput.lpJoystick.emplace_back(_lpJoystick);

	// 最初の1つのみで終わる
	return DIENUM_CONTINUE;			// 次のデバイスを列挙するにはDIENUM_CONTINUEを返す
}

bool DInput::InitDirectInput()
{
	for (int i = 0; i < TRG_NUM_MAX; i++)
	{
		for (auto& it : triggerCount[i])
		{
			it = 0;
		}
	}

	// IDirectInput8の作成
	// TODO 追加3 : mainで取得したinstanceをここで使う。
	HRESULT hr = DirectInput8Create(instance, DIRECTINPUT_VERSION, IID_IDirectInput8, (LPVOID*)&lpDI, NULL);
	assert(SUCCEEDED(hr) && "GamePad.cpp Line : 266");

	// ジョイスティックの列挙
	hr = lpDI->EnumDevices(DI8DEVCLASS_GAMECTRL, EnumJoyDeviceProc, NULL, DIEDFL_ATTACHEDONLY);
	assert(SUCCEEDED(hr) && "GamePad.cpp Line : 266");

	int num = 0;

	for (auto& joy : lpJoystick)
	{
		if (!joy) {
			return false;
		}

		// デバイス情報
		dc.dwSize = sizeof(dc);
		joy->GetCapabilities(&dc);

		// 動作開始
		joy->Acquire();

		num++;
	}

	js.resize(num);

	return true;
}

void DInput::UpdateDirectInput()
{
	for (int i = 0; i < lpJoystick.size();i++)
	{
		if (!lpJoystick[i]) return;

		// データ取得前にPollが必要なら
		if (dc.dwFlags&DIDC_POLLEDDATAFORMAT) {
			lpJoystick[i]->Poll();
		}

		// ジョイスティックの入力
		ZeroMemory(&js[i], sizeof(js[i]));
		HRESULT ret = lpJoystick[i]->GetDeviceState(sizeof(js[i]), &js[i]);
		if (FAILED(ret)) {
			// 失敗なら再び動作開始を行う
			lpJoystick[i]->Acquire();
		}
	}
}

JoystickState GetJoystickState(const int num)
{
	int num_ = 1 - num;
	JoystickState ans;
//	if (dInput.js.size() - 1 != num)
//	{
//		ans.x = 0;
//		ans.y = 0;
//		ans.Rx = 0;
//		ans.Ry = 0;
//		return ans;
//	}
	// X
	if (dInput.js[num].lX >= -DEADZONE && dInput.js[num].lX <= DEADZONE)
	{
		ans.x = 0;
	}
	else
	{
		ans.x = dInput.js[num].lX;
	}

	//Y
	if (dInput.js[num].lY >= -DEADZONE && dInput.js[num].lY <= DEADZONE)
	{
		ans.y = 0;
	}
	else
	{
		ans.y = dInput.js[num].lY;
	}

	//Rx
	if (dInput.js[num].lRx >= -DEADZONE && dInput.js[num].lRx <= DEADZONE)
	{
		ans.Rx = 0;
	}
	else
	{
		ans.Rx = dInput.js[num].lRx;
	}

	//Ry
	if (dInput.js[num].lRy >= -DEADZONE && dInput.js[num].lRy <= DEADZONE)
	{
		ans.Ry = 0;
	}
	else
	{
		ans.Ry = dInput.js[num].lRy;
	}

	return ans;
}

bool DPadInputState(int _direction, int _num)
{
	JoystickState d = GetJoystickState(_num);
	switch (_direction)
	{
	case STICK_UP:
		if (d.y <= -DIRECTION_LIMIT)
		{
			return true;
		}
		return false;
		break;
	case STICK_RIGHT:
		if (d.x >= DIRECTION_LIMIT)
		{
			return true;
		}
		return false;
		break;
	case STICK_DOWN:
		if (d.y >= DIRECTION_LIMIT)
		{
			return true;
		}
		return false;
		break;
	case STICK_LEFT:
		if (d.x <= -DIRECTION_LIMIT)
		{
			return true;
		}
		return false;
		break;
	}
	return false;
}

bool DPadInputTrigger(int _direction, int _num)
{
	if (DPadInputState(_direction, _num))
	{
		if (dInput.dPadTriggerCount[_direction] == 1)
		{
			return true;
		}
	}
	return false;
}