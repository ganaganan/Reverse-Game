#include	"Sound.h"
#include	"easing.h"
#include	"BaseScene.h"
using namespace easing;

// TODO : Sound 変更 saTimer -> playbackTime
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
//	初期化関数
/*------------------------------------*/
void Sound::Init()
{
	count = 0;
	playbackTime = 0;
	soundType = SoundType::Max;

	rainSound								= new Audio("Data/Sound/Rain.wav");
	aroundStageSound[SoundType::Thunder]	= new Audio("Data/Sound/雷.wav");
	aroundStageSound[SoundType::Wind]		= new Audio("Data/Sound/風で木の葉が揺れる音.wav");
	aroundStageSound[SoundType::Chime]		= new Audio("Data/Sound/チャイム.wav");
	aroundStageSound[SoundType::Ambulance]	= new Audio("Data/Sound/救急車.wav");
	aroundStageSound[SoundType::Footsteps]	= new Audio("Data/Sound/通り過ぎる敵の足音.wav");	// TODO : Sound 変更 path name 歩く音ブーツ(?) -> 通り過ぎる敵の足音
	aroundStageSound[SoundType::Pass]		= new Audio("Data/Sound/通りゃんせ.wav");

	rainSound->SetPosition(XMFLOAT3(0, 0, 0));
	rainSound->SetVolume(0.5f);
	rainSound->Play(true);
}

/*------------------------------------*/
//	更新関数
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
	case 0:	// いちんちめ
		soundType = SoundType::Thunder;
		break;

	case 1:	// ににちめ
		while (soundType == SoundType::Chime || soundType == SoundType::Pass)
		{
			soundType = rand() % SoundType::Max;
		}
		break;

	case 2:	// さんにちめ
		break;

	default:
		assert(BaseScene::GetNowDay() > 2 && "なんにちめ？");
		break;
	}

	aroundStageSound[soundType]->SetPosition(position);
	aroundStageSound[soundType]->Play(false);
	playbackTime = 0;

//	count = saTimer + PLAY_SOUND_PER_SECOND;						// 一定時間おきに音鳴らす方
	count = static_cast<int>(playTime[soundType]) + rand() % 3 * 60 + (PLAY_SOUND_PER_SECOND - 1 * 60);	// 乱数を用いる方
}

/*------------------------------------*/
//	音の減衰関数	(終わりにかけて小さく)
/*------------------------------------*/
void Sound::VolumeDecay()	// TODO : Sound 変更 関数名 VolumeAttention(?) -> VolumeDecay
{
	float volume;
	switch (soundType)
	{
	// 減衰させるやつだけあれする
	case SoundType::Wind:
		volume = InExp(playbackTime, playTime[soundType], 0.0f, 1.0f);
		if (volume < 0)	volume = 0;
		aroundStageSound[soundType]->SetVolume(volume);
		break;

		// TODO : Sound 削除 case SoundType::Chime

	default:	break;
	}
	playbackTime++;	// TODO : Sound 追加 or 変更 playbackTimeをインクリメントする
}