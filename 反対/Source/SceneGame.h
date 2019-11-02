#pragma once

#include "Scene.h"

class SceneGame : public BaseScene
{
public:
	SceneGame(){}
	void Init();
	void Uninit();
	void Update();
	void Render();
};

extern SceneGame sceneGame;