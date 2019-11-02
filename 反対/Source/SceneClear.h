#pragma once

#include "Scene.h"

class SceneClear : public BaseScene
{
public:
	void Init();
	void Uninit();
	void Update();
	void Render();
};

extern SceneClear sceneClear;