#ifndef INCLUDED_COMMON
#define	INCLUDED_COMMON

//******************************************************************************
//
//
//      common
//
//
//******************************************************************************

//------< インクルード >---------------------------------------------------------
#include <DirectXMath.h>

//------< 関数 >----------------------------------------------------------------

static float(*const ToRadian)(float) = DirectX::XMConvertToRadians;  // 角度をラジアンに
static float(*const ToDegree)(float) = DirectX::XMConvertToDegrees;  // ラジアンを角度に

#endif // !INCLUDED_COMMON
