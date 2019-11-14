#pragma once

#include "Scene.h"
#include "../MyLib/Audio.h"

class SceneOver : public BaseScene
{
	enum SoundType
	{
		Shout,
		Max,
	};
	Audio* sound[SoundType::Max];


	int enemyType;
	int lastDay;
public:
	SceneOver() {}

	void Init();
	void Uninit();
	void Update();
	void Render();
};

extern SceneOver sceneOver;