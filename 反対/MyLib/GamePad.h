#ifndef _GAMEPAD_H_
#define _GAMEPAD_H_

#include<d3d11.h>
#include<Xinput.h>
#include<sstream>
#include <map>
#include <Windows.h>

#include <dinput.h>
#include <vector>

#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")


//-----------------------------------------------------------------------------
// structs
//-----------------------------------------------------------------------------
struct CONTROLLER_STATE
{
	XINPUT_STATE state;
	bool bConnected;
};

//-----------------------------------------------------------------------------
// rabel
//-----------------------------------------------------------------------------
enum BUTTONS
{
	DPAD_UP,
	DPAD_DOWN,
	DPAD_LEFT,
	DPAD_RIGHT,
	START,
	BACK,
	LEFT_THUMB,
	RIGHT_THUMB,
	LEFT_SHOULDER,
	RIGHT_SHOULDER,
	A,
	B,
	X,
	Y,
	DPAD_END,
};

//-----------------------------------------------------------------------------
// constants
//-----------------------------------------------------------------------------
#define MAX_CONTROLLERS			4

#define XINPUT_DPAD_UP			0x0001	// 1
#define XINPUT_DPAD_DOWN		0x0002	// 2
#define XINPUT_DPAD_LEFT		0x0004	// 4
#define XINPUT_DPAD_RIGHT		0x0008	// 8
#define XINPUT_START			0x0010	// 16
#define XINPUT_BACK				0x0020	// 32
#define XINPUT_LEFT_THUMB		0x0040	// 64
#define XINPUT_RIGHT_THUMB		0x0080	// 128
#define XINPUT_LEFT_SHOULDER	0x0100	// 256
#define XINPUT_RIGHT_SHOULDER	0x0200	// 512
#define XINPUT_A				0x1000	// 4096
#define XINPUT_B				0x2000	// 8192
#define XINPUT_X				0x4000	// 16384
#define XINPUT_Y				0x8000	// 32768

#define TRG_NUM_MAX 2
//-----------------------------------------------------------------------------
// Variables
//-----------------------------------------------------------------------------
extern int triggerCount[TRG_NUM_MAX][BUTTONS::DPAD_END];
extern std::map<int, int> checkButton;

//-----------------------------------------------------------------------------
// extern variables and fanctions
//-----------------------------------------------------------------------------
/*variables*/
extern CONTROLLER_STATE controllers[MAX_CONTROLLERS];

/*fanctions*/
WORD ThumbToDPadLeft( SHORT sThumbX, SHORT sThumbY, SHORT sDeadZone );

// DirectInput
#define DEADZONE 100

//-----------------------------------------------------------------------------
// class and structs
//-----------------------------------------------------------------------------
#define DIRECTION_LIMIT 900
enum
{
	STICK_UP,
	STICK_RIGHT,
	STICK_DOWN,
	STICK_LEFT,
};

class DInput
{
public:
	HWND					hwnd = NULL;
	HINSTANCE				instance = NULL; // TODO �ǉ�1 : hwnd�Ɠ����ŃE�B���h�E�쐬���Ɏ擾����B�ǉ��������Ƃň������Ȃ����܂����B(TODO �ǉ�2��)
	LPDIRECTINPUT8			lpDI = NULL;		// IDirectInput8
	std::vector<LPDIRECTINPUTDEVICE8> lpJoystick;
	//LPDIRECTINPUTDEVICE8	lpJoystick[2] = { NULL };	// �W���C�X�e�B�b�N�f�o�C�X
	DIDEVCAPS dc;
	int dPadTriggerCount[4];
public:
	std::vector<DIJOYSTATE> js;

public:
	~DInput()
	{
		for (auto& joy : lpJoystick)
		{
			if (joy)
			{
				joy->Release();
				joy = NULL;
			}
		}
		if (lpDI)
		{
			lpDI->Release();
			lpDI = NULL;
		}
	}
	bool InitDirectInput();
	void UpdateDirectInput();
};
struct JoystickState
{
	float x;
	float y;
	float Rx;
	float Ry;
};
BOOL PASCAL EnumJoyDeviceProc(LPCDIDEVICEINSTANCE lpddi, LPVOID pvRef);
extern DInput dInput;

extern JoystickState GetJoystickState(const int num);
extern bool			 DPadInputState(int _direction, int _num = 0);
extern bool			 DPadInputTrigger(int _direction, int _num = 0);

// example
extern HRESULT InitControllers();
extern HRESULT UpdateControllerState();
extern void		RenderFrame();
extern bool		InputTrigger(int, int _padNum = 0);
extern bool		InputState(int, int _padNum = 0);

void ChackInputState();


#endif //_GAMEPAD_H_
