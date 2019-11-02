#include "SceneTitle.h"

#include <Windows.h>

void SceneTitle::Init()
{

}

void SceneTitle::Update()
{
	if (GetAsyncKeyState(VK_SPACE) < 0)
	{
		SceneManager::Get().SetScene(SceneManager::Get().MAIN_GAME);
	}
}

void SceneTitle::Render()
{

}

void SceneTitle::Uninit()
{

}