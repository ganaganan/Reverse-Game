#pragma once

#include "Scene.h"
#include "../MyLib/Audio.h"

class SceneTitle : public BaseScene
{
	static constexpr int FADE_TIME = 60 * 1;

	enum SoundTypeDebug
	{
		Rain,
		Thunder_title,
		Thunderstorm,
		Chaim,
		OpenDoor,
		Rain2,
		Pass,
		RingTone,
		Wind,
		Walk,
		Thunder_game,
		Daruma,
		Ghost,
		pole,
		Max,
	};
	Audio*  debugSound[SoundTypeDebug::Max];
	float	debugSoundVol[SoundTypeDebug::Max];

//	enum SoundType
//	{
//		Rain,			// ‰J
//		Thunder,		// —‹
//		Thunderstorm,	// —‹‰J
//		Max,
//	};
//	Audio*	sound[SoundType::Max];
	int		soundType;		// SoundType - Rain or Thunderstorm

	int		state;

public:
	SceneTitle(){}

	void Init();
	void Uninit();
	void Update();
	void Render();
};

extern SceneTitle sceneTitle;