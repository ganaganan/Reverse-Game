#include	"Sound.h"
#include	"easing.h"
#include	"BaseScene.h"
using namespace easing;

// TODO : Sound �ύX saTimer -> playbackTime
Sound::Sound()
{

}

Sound::~Sound()
{
	delete rainSound;
	rainSound = nullptr;

	for (auto &sound : aroundStageSound)
	{
		delete sound;
		sound = nullptr;
	}
}

/*------------------------------------*/
//	�������֐�
/*------------------------------------*/
void Sound::Init()
{
	count = 0;
	playbackTime = 0;
	soundType = SoundType::Max;

	rainSound								= new Audio("Data/Sound/Rain.wav");
	aroundStageSound[SoundType::Thunder]	= new Audio("Data/Sound/��.wav");
	aroundStageSound[SoundType::Wind]		= new Audio("Data/Sound/���Ŗ؂̗t���h��鉹.wav");
	aroundStageSound[SoundType::Chime]		= new Audio("Data/Sound/�`���C��.wav");
	aroundStageSound[SoundType::Ambulance]	= new Audio("Data/Sound/�~�}��.wav");
	aroundStageSound[SoundType::Footsteps]	= new Audio("Data/Sound/�ʂ�߂���G�̑���.wav");	// TODO : Sound �ύX path name �������u�[�c(?) -> �ʂ�߂���G�̑���
	aroundStageSound[SoundType::Pass]		= new Audio("Data/Sound/�ʂ���.wav");

	rainSound->SetPosition(XMFLOAT3(0, 0, 0));
	rainSound->SetVolume(0.5f);
	rainSound->Play(true);
}

/*------------------------------------*/
//	�X�V�֐�
/*------------------------------------*/
void Sound::Update()
{
	if (count-- > 0)
	{
		VolumeDecay();
		return;
	}

	XMFLOAT3	position(0.0f, 0.0f, 0.0f);
	position.x = static_cast<float>(rand() % static_cast<int>(Audio::SIDE_MAX * 2)) - Audio::SIDE_MAX;
	position.z = static_cast<float>(rand() % static_cast<int>(Audio::FRONT_MAX));

	soundType = rand() % SoundType::Max;

	int date = BaseScene::GetNowDay();
	switch (date)
	{
	case 0:	// �����񂿂�
		soundType = SoundType::Thunder;
		break;

	case 1:	// �ɂɂ���
		while (soundType == SoundType::Chime || soundType == SoundType::Pass)
		{
			soundType = rand() % SoundType::Max;
		}
		break;

	case 2:	// ����ɂ���
		break;

	default:
		assert(BaseScene::GetNowDay() > 2 && "�Ȃ�ɂ��߁H");
		break;
	}

	aroundStageSound[soundType]->SetPosition(position);
	aroundStageSound[soundType]->Play(false);
	playbackTime = 0;

//	count = saTimer + PLAY_SOUND_PER_SECOND;						// ��莞�Ԃ����ɉ��炷��
	count = static_cast<int>(playTime[soundType]) + rand() % 3 * 60 + (PLAY_SOUND_PER_SECOND - 1 * 60);	// ������p�����
}

/*------------------------------------*/
//	���̌����֐�	(�I���ɂ����ď�����)
/*------------------------------------*/
void Sound::VolumeDecay()	// TODO : Sound �ύX �֐��� VolumeAttention(?) -> VolumeDecay
{
	float volume;
	switch (soundType)
	{
	// �����������������ꂷ��
	case SoundType::Wind:
		volume = InExp(playbackTime, playTime[soundType], 0.0f, 1.0f);
		if (volume < 0)	volume = 0;
		aroundStageSound[soundType]->SetVolume(volume);
		break;

		// TODO : Sound �폜 case SoundType::Chime

	default:	break;
	}
	playbackTime++;	// TODO : Sound �ǉ� or �ύX playbackTime���C���N�������g����
}