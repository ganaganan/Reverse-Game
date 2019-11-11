#include "SceneTitle.h"

#include <Windows.h>

#include "../MyLib/GamePad.h"

void SceneTitle::Init()
{

}

void SceneTitle::Update()
{
	if (GetAsyncKeyState(VK_SPACE) < 0 || InputTrigger(XINPUT_A))
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