#include "SceneOver.h"

#include <framework.h>
#include "../MyLib/GamePad.h"

void SceneOver::Init()
{
	sound[SoundType::Shout] = new Audio("Data/Sound/GAMEOVER.wav");
	sound[SoundType::Shout]->Play(false);
}

void SceneOver::Update()
{
	if (InputTrigger(XINPUT_X))
	{
		switch (nowDay)
		{
		case 0:
			SceneManager::Get().SetScene(SceneManager::MAIN_GAME);	break;
		case 1:
			SceneManager::Get().SetScene(SceneManager::MAIN_GAME2);	break;
		case 2:
			SceneManager::Get().SetScene(SceneManager::MAIN_GAME3);	break;
		}
	}
	else if (InputTrigger(XINPUT_B))
	{
		SceneManager::Get().SetScene(SceneManager::TITLE);
	}
#ifdef USE_IMGUI
		ImGui::Begin(" game over ");
		ImGui::Text("Retry : X   Title : B");
		ImGui::End();
#endif

}

void SceneOver::Render()
{

}

void SceneOver::Uninit()
{
	for (auto& it : sound)
	{
		delete it;
		it = nullptr;
	}
}