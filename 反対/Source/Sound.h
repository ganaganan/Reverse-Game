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
		Rain,		// �J
		Thunder,	// ��
		Wind,		// ��
		Chime,		// �`���C��
		Ringtone,	// �d�b
		Footsteps,	// ����
		Pass,		// �ʂ���
		Max,
	};

private:
	Audio*	aroundStageSound[SoundType::Max];
	const float	playTime[SoundType::Max] =
	{
		60 * 11,	// �J
		60 * 6,		// ��
		60 * 3,		// ��
		60 * 16,	// �`���C��
		60 * 4,		// �d�b
		60 * 2,		// ����
		60 * 9,		// �ʂ���
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