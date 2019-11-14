#include "SceneTitle.h"

#include <Windows.h>

#include "../MyLib/GamePad.h"
#include "easing.h"
using namespace easing;

#define	UseThunderstorm	0

void SceneTitle::Init()
{
//	sound[SoundType::Rain]			= new Audio("Data/Sound/Rain.wav");
//	sound[SoundType::Thunder]		= new Audio("Data/Sound/Thunder.wav");
//	sound[SoundType::Thunderstorm]	= new Audio("Data/Sound/title.wav");

	debugSound[SoundTypeDebug::Rain] = new Audio("Data/Sound/Rain.wav");
	debugSound[SoundTypeDebug::Thunder_title] = new Audio("Data/Sound/Thunder.wav");
	debugSound[SoundTypeDebug::Thunderstorm] = new Audio("Data/Sound/title.wav");
	debugSound[SoundTypeDebug::Chaim] = new Audio("Data/Sound/チャイム.wav");
	debugSound[SoundTypeDebug::OpenDoor] = new Audio("Data/Sound/ドアの音.wav");
	debugSound[SoundTypeDebug::Rain2] = new Audio("Data/Sound/雨.wav");
	debugSound[SoundTypeDebug::Pass] = new Audio("Data/Sound/通りゃんせ.wav");
	debugSound[SoundTypeDebug::RingTone] = new Audio("Data/Sound/電話.wav");
	debugSound[SoundTypeDebug::Wind] = new Audio("Data/Sound/風で木の葉が揺れる音.wav");
	debugSound[SoundTypeDebug::Walk] = new Audio("Data/Sound/歩く音ブーツ.wav");
	debugSound[SoundTypeDebug::Thunder_game] = new Audio("Data/Sound/雷.wav");
	debugSound[SoundTypeDebug::Daruma] = new Audio("Data/Sound/だるま.wav");
	debugSound[SoundTypeDebug::Ghost] = new Audio("Data/Sound/幽霊飛んでる音.wav");
	debugSound[SoundTypeDebug::pole] = new Audio("Data/Sound/重い金属引きずる音.wav");

	for (auto& it : debugSoundVol)
		it = 1.0f;

#ifndef UseThunderstorm
	soundType = SoundType::Thunderstorm;
#else
//	soundType = SoundType::Rain;
#endif
//	sound[soundType]->Play(true);

	state = 0;
}

void SceneTitle::Update()
{
	static int timer = 0;
	float volume;

	switch (state)
	{
	case 0:		// default title
#ifndef UseThunderstorm
#else
		if (timer-- < 0)
		{
			timer = rand() % 60 + 60 * 8;
//			sound[SoundType::Thunder]->Play(false);
		}
#endif
		if (GetAsyncKeyState(VK_SPACE) < 0 || InputTrigger(XINPUT_A))
		{
			timer = 0;
			state++;
		}
		break;

	case 1:		// fade out
		volume = InExp(timer++, FADE_TIME, 0.0f, 1.0f);
		if (volume < 0)
		{
			volume = 0;
			state++;
		}
//		sound[soundType]->SetVolume(volume);
		break;

	case 2:		// set next scene
		SceneManager::Get().SetScene(SceneManager::Get().MAIN_GAME);
	default:	break;
	}
#ifdef USE_IMGUI
	ImGui::Begin("Scene Title");
	ImGui::Text(" Next : A ");

	ImGui::NewLine();

	if (ImGui::TreeNode("PlaySound and Change vol"))
	{
		if (ImGui::Button("Rain")) 
			debugSound[SoundTypeDebug::Rain]->Play(false);
		if (ImGui::Button("Thunder_title")) 
			debugSound[SoundTypeDebug::Thunder_title]->Play(false);
		if (ImGui::Button("Thunderstorm")) 
			debugSound[SoundTypeDebug::Thunderstorm]->Play(false);
		if (ImGui::Button("Chaim")) 
			debugSound[SoundTypeDebug::Chaim]->Play(false);
		if (ImGui::Button("OpenDoor")) 
			debugSound[SoundTypeDebug::OpenDoor]->Play(false);
		if (ImGui::Button("Rain2")) 
			debugSound[SoundTypeDebug::Rain2]->Play(false);
		if (ImGui::Button("Pass")) 
			debugSound[SoundTypeDebug::Pass]->Play(false);
		if (ImGui::Button("RingTone")) 
			debugSound[SoundTypeDebug::RingTone]->Play(false);
		if (ImGui::Button("Wind")) 
			debugSound[SoundTypeDebug::Wind]->Play(false);
		if (ImGui::Button("Walk")) 
			debugSound[SoundTypeDebug::Walk]->Play(false);
		if (ImGui::Button("Thunder_game")) 
			debugSound[SoundTypeDebug::Thunder_game]->Play(false);
		if (ImGui::Button("Daruma")) 
			debugSound[SoundTypeDebug::Daruma]->Play(false);
		if (ImGui::Button("Ghost")) 
			debugSound[SoundTypeDebug::Ghost]->Play(false);
		if (ImGui::Button("pole")) 
			debugSound[SoundTypeDebug::pole]->Play(false);

		ImGui::SliderFloat("Rain", &debugSoundVol[SoundTypeDebug::Rain], 0.0f, 1.0f);
		ImGui::SliderFloat("Thunderstorm", &debugSoundVol[SoundTypeDebug::Thunderstorm], 0.0f, 1.0f);
		ImGui::SliderFloat("Chaim", &debugSoundVol[SoundTypeDebug::Chaim], 0.0f, 1.0f);
		ImGui::SliderFloat("OpenDoor", &debugSoundVol[SoundTypeDebug::OpenDoor], 0.0f, 1.0f);
		ImGui::SliderFloat("Rain2", &debugSoundVol[SoundTypeDebug::Rain2], 0.0f, 1.0f);
		ImGui::SliderFloat("Pass", &debugSoundVol[SoundTypeDebug::Pass], 0.0f, 1.0f);
		ImGui::SliderFloat("RingTone", &debugSoundVol[SoundTypeDebug::RingTone], 0.0f, 1.0f);
		ImGui::SliderFloat("Wind", &debugSoundVol[SoundTypeDebug::Wind], 0.0f, 1.0f);
		ImGui::SliderFloat("Walk", &debugSoundVol[SoundTypeDebug::Walk], 0.0f, 1.0f);
		ImGui::SliderFloat("Thunder_game", &debugSoundVol[SoundTypeDebug::Thunder_game], 0.0f, 1.0f);
		ImGui::SliderFloat("Daruma", &debugSoundVol[SoundTypeDebug::Daruma], 0.0f, 1.0f);
		ImGui::SliderFloat("Ghost", &debugSoundVol[SoundTypeDebug::Ghost], 0.0f, 1.0f);
		ImGui::SliderFloat("pole", &debugSoundVol[SoundTypeDebug::pole], 0.0f, 1.0f);
		ImGui::TreePop();
	}
	ImGui::End();
#endif
}

void SceneTitle::Render()
{

}

void SceneTitle::Uninit()
{
//	for (auto &sound : sound)
//	{
//		delete sound;
//		sound = nullptr;
//	}
	for (auto& it : debugSound)
	{
		delete it;
		it = nullptr;
	}
}