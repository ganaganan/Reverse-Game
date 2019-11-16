#include "Scene.h"

#include <Audio.h>

#include "SceneOver.h"
//#include <Audio.cpp>

int BaseScene::lastEnemyType;
int BaseScene::nowDay;

SceneTitle sceneTitle;
SceneGame sceneGame;
SceneGame2 sceneGame2;
SceneGame3 sceneGame3;
SceneClear sceneClear;
SceneOver  sceneOver;
SceneTutorial sceneTutorial;


BaseScene* scene_tbl[] = {
	&sceneTitle,
	&sceneTutorial,
	&sceneGame,
	&sceneGame2,
	&sceneGame3,
	&sceneClear,
	&sceneOver,
};


void SceneManager::Init()
{
//	pAudio->Initialize();
	SetScene(SCENE::TITLE);
}

void SceneManager::Update()
{
	if (nextScene)
	{
		if (nowScene)
		{
			nowScene->Uninit();
		}
		nowScene = nextScene;
		nextScene = nullptr;
		nowScene->Init();
	}
	nowScene->Update();
}

void SceneManager::Render()
{
	nowScene->Render();
}

void SceneManager::Uninit()
{
	nowScene->Uninit();
	if (nowScene)
	{
		//delete nowScene;
	}
	if (nextScene)
	{
		delete nextScene;
	}

	nowScene = nullptr;
	nextScene = nullptr;

//	pAudio->Uninitialize();
}

void SceneManager::SetScene(SCENE _sceneNum)
{
	nextScene = scene_tbl[_sceneNum];
}