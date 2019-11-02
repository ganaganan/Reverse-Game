#pragma once

#include<d3d11.h>
#include<cassert>
#include"framework.h"

/********************************************************************

	Blender

********************************************************************/
enum BLEND_MODE
{
	NONE, ALPHA, ADD, SUBTRACT, MULTIPLY,
	END
};

extern void SetDrawBlendMode(BLEND_MODE _mode);


class Blender
{//multiplication
public:

	static ID3D11BlendState*	modes[END];

public:
	Blender(ID3D11Device*);
	~Blender()
	{
		for (auto& p : modes)
		{
			p->Release();
			p = nullptr;
		}
	}

	static void SetBlendMode(ID3D11DeviceContext* _deviceContext, BLEND_MODE blend)
	{
		_deviceContext->OMSetBlendState(modes[blend], nullptr, 0xFFFFFFFF);
	}
};



