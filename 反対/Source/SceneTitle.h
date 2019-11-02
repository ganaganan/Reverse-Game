#pragma once

#include "Scene.h"

class SceneTitle : public BaseScene
{
public:
	SceneTitle(){}

	void Init();
	void Uninit();
	void Update();
	void Render();
};

extern SceneTitle sceneTitle;