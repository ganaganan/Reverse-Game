#include "Scene.h"

#include <Audio.h>
//#include <Audio.cpp>

SceneTitle sceneTitle;
SceneGame sceneGame;
SceneClear sceneClear;

BaseScene* scene_tbl[] = {
	&sceneTitle,
	&sceneGame,
	&sceneClear,
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
			nowScene = nextScene;
			nextScene = nullptr;
			nowScene->Init();
		}
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