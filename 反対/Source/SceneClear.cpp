#include "SceneClear.h"

#include <framework.h>

#include "../MyLib/GamePad.h"

void SceneClear::Init()
{

}

void SceneClear::Update()
{
#ifdef USE_IMGUI
	ImGui::Begin("Scene Clear");

	ImGui::Text("Go Title : A");

	ImGui::End();
#endif 

	if (InputTrigger(XINPUT_A))
	{
		SceneManager::Get().SetScene(SceneManager::TITLE);
	}
}

void SceneClear::Render()
{

}

void SceneClear::Uninit()
{

}