#include	"Sound.h"
#include	"easing.h"
using namespace easing;


Sound::Sound()
{

}

Sound::~Sound()
{
	for (auto &sound : aroundStageSound)
	{
		delete sound;
		sound = nullptr;
	}
}

/*------------------------------------*/
//	初期化関数
/*------------------------------------*/
void Sound::Init()
{
	count = 0;
	saTimer = 0;
	soundType = SoundType::Max;

	aroundStageSound[SoundType::Rain]		= new Audio("Data/Sound/雨.wav");
	aroundStageSound[SoundType::Thunder]	= new Audio("Data/Sound/雷.wav");
	aroundStageSound[SoundType::Wind]		= new Audio("Data/Sound/風で木の葉が揺れる音.wav");
	aroundStageSound[SoundType::Chime]		= new Audio("Data/Sound/チャイム.wav");
	aroundStageSound[SoundType::Ringtone]	= new Audio("Data/Sound/電話.wav");
	aroundStageSound[SoundType::Footsteps]	= new Audio("Data/Sound/歩く音ブーツ.wav");
	aroundStageSound[SoundType::Pass]		= new Audio("Data/Sound/通りゃんせ.wav");
}

/*------------------------------------*/
//	更新関数
/*------------------------------------*/
void Sound::Update()
{
#if 0
	if (count-- > 0)
	{
		VolumeAttenuation();
		return;
	}

	XMFLOAT3	position(0.0f, 0.0f, 0.0f);
	position.x = static_cast<float>(rand() % static_cast<int>(Audio::SIDE_MAX * 2)) - Audio::SIDE_MAX;
	position.z = static_cast<float>(rand() % static_cast<int>(Audio::FRONT_MAX));

	soundType = rand() % SoundType::Max;

	aroundStageSound[soundType]->SetPosition(position);
	aroundStageSound[soundType]->Play(false);
	saTimer = 0;

//	count = saTimer + PLAY_SOUND_PER_SECOND;						// 一定時間おきに音鳴らす方
	count = static_cast<int>(playTime[soundType]) + rand() % 3 * 60 + (PLAY_SOUND_PER_SECOND - 1 * 60);	// 乱数を用いる方
#else
	if (GetAsyncKeyState(' ') & 1)
	{
		XMFLOAT3	position(0.0f, 0.0f, 0.0f);
		position.x = static_cast<float>(rand() % static_cast<int>(Audio::SIDE_MAX * 2)) - Audio::SIDE_MAX;
		position.z = 0.0f;//static_cast<float>(rand() % static_cast<int>(Audio::FRONT_MAX));

		soundType = rand() % SoundType::Max;

		aroundStageSound[SoundType::Rain]->SetPosition(position);
		aroundStageSound[SoundType::Rain]->Play(true);
	}
#endif
}

/*------------------------------------*/
//	音の減衰関数	(終わりにかけて小さく)
/*------------------------------------*/
void Sound::VolumeAttenuation()
{
	float volume;
	switch (soundType)
	{
	// 減衰させるやつだけあれする
	case SoundType::Rain:
	case SoundType::Wind:
		volume = InExp(saTimer, playTime[soundType], 0.0f, 1.0f);
		if (volume < 0)	volume = 0;
		aroundStageSound[soundType]->SetVolume(volume);
		break;
	case SoundType::Chime:
		volume = 0.0f;
		break;
	default:	break;
	}
}