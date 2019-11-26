#pragma once

#include <DirectXMath.h>
#include <Vector.h>

#include "../MyLib/Audio.h"
#include "framework.h"

class Sound
{
private:
	static constexpr int	PLAY_SOUND_PER_SECOND = 8 * 60;
public:	// TODO : Sound 追加 SoundTypeのみpublic
	enum SoundType
	{
//		Rain,		// 雨
		Thunder,	// 雷
		Wind,		// 風
		Chime,		// チャイム
		Ambulance,	// 救急車
		Footsteps,	// 足音
		Pass,		// 通りゃんせ
		Max,
	};

private:
	Audio*	rainSound;
	Audio*	aroundStageSound[SoundType::Max];
	const float	playTime[SoundType::Max] =
	{
//		60 * 11,	// 雨
		60 * 6,		// 雷
		60 * 3,		// 風
		60 * 16,	// チャイム
		60 * 4,		// 電話
		60 * 2,		// 足音
		60 * 9,		// 通りゃんせ
	};

	int		soundType;
	int		count;

public:
	// TODO : Sound 変数名の変更 saTimer -> playbackTime
	int		playbackTime;		// 現在の再生時間(frame)

public:
	virtual	~Sound();
	void	Init();
	void	Update();
	// TODO : Sound ゲッターの追加
	int		GetSoundType() { return soundType; }

private:
	Sound();
	void	VolumeDecay();	// TODO : Sound 関数名の変更 VolumeAttention(?) -> VolumeDecay

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