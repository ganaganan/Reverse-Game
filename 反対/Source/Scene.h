#pragma once

#include "BaseScene.h"
#include "SceneTitle.h"
#include "SceneGame.h"
#include "SceneClear.h"
#include "SceneTutorial.h"

class SceneManager
{
public:
	BaseScene* nowScene;
	BaseScene* nextScene;

public:
	enum SCENE
	{
		TITLE,
		TUTORIAL,
		MAIN_GAME,
		MAIN_GAME2,
		MAIN_GAME3,
		GAME_CLEAR,
		GAME_OVER,
	};

public:
	void Init();
	void Uninit();
	void Update();
	void Render();

	void SetScene(SCENE _sceneNum);

	static SceneManager& Get()
	{
		static SceneManager instance;
		return instance;
	}
};

