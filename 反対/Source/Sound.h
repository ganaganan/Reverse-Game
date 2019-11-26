#pragma once

#include <DirectXMath.h>
#include <Vector.h>

#include "../MyLib/Audio.h"
#include "framework.h"

class Sound
{
private:
	static constexpr int	PLAY_SOUND_PER_SECOND = 8 * 60;
public:	// TODO : Sound �ǉ� SoundType�̂�public
	enum SoundType
	{
//		Rain,		// �J
		Thunder,	// ��
		Wind,		// ��
		Chime,		// �`���C��
		Ambulance,	// �~�}��
		Footsteps,	// ����
		Pass,		// �ʂ���
		Max,
	};

private:
	Audio*	rainSound;
	Audio*	aroundStageSound[SoundType::Max];
	const float	playTime[SoundType::Max] =
	{
//		60 * 11,	// �J
		60 * 6,		// ��
		60 * 3,		// ��
		60 * 16,	// �`���C��
		60 * 4,		// �d�b
		60 * 2,		// ����
		60 * 9,		// �ʂ���
	};

	int		soundType;
	int		count;

public:
	// TODO : Sound �ϐ����̕ύX saTimer -> playbackTime
	int		playbackTime;		// ���݂̍Đ�����(frame)

public:
	virtual	~Sound();
	void	Init();
	void	Update();
	// TODO : Sound �Q�b�^�[�̒ǉ�
	int		GetSoundType() { return soundType; }

private:
	Sound();
	void	VolumeDecay();	// TODO : Sound �֐����̕ύX VolumeAttention(?) -> VolumeDecay

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