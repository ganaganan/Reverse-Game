#pragma once

#include <DirectXMath.h>
#include <Vector.h>

#include "../MyLib/Audio.h"

class Sound
{
private:
	static constexpr int	PLAY_SOUND_PER_SECOND = 8 * 60;
//	static constexpr float	RIGHT_MAX = 100;
//	static constexpr float	LEFT_MAX = -100;
//	static constexpr float	FRONT_MAX = 1000;
	enum SoundType
	{
		Rain,		// 雨
		Thunder,	// 雷
		Wind,		// 風
		Chime,		// チャイム
		Ringtone,	// 電話
		Footsteps,	// 足音
		Pass,		// 通りゃんせ
		Max,
	};

private:
	Audio*	aroundStageSound[SoundType::Max];
	const float	playTime[SoundType::Max] =
	{
		60 * 11,	// 雨
		60 * 6,		// 雷
		60 * 3,		// 風
		60 * 16,	// チャイム
		60 * 4,		// 電話
		60 * 2,		// 足音
		60 * 9,		// 通りゃんせ
	};

	int		soundType;
	int		count;
	float	saTimer;

public:
	virtual	~Sound();
	void	Init();
	void	Update();

private:
	Sound();
	void	VolumeAttenuation();

#ifdef USE_IMGUI
	void	UseImGui() {}
#endif


public:
	static Sound& Get()
	{
		static Sound instance;
		return instance;
	}
};