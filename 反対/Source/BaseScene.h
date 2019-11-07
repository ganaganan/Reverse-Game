#pragma once

#include <DirectXMath.h>

class BaseScene
{
protected:
	DirectX::XMFLOAT3 lightDir;

public:
	virtual void Init() = 0;
	virtual void Uninit() = 0;
	virtual void Update() = 0;
	virtual void Render() = 0;
};
