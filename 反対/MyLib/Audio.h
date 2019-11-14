#ifndef		_WAVEFILE_H_
#define		_WAVEFILE_H_

#include	<stdio.h>
#include	<Windows.h>
#include	<al.h>
#include	<alc.h>
#include	<DirectXMath.h>
#pragma comment(lib, "OpenAL32.lib")

using namespace DirectX;

class Audio
{
	// �`�����N�����ʂ���ׂ̒l
	static const unsigned int CHUNK_RIFF_TAG = 'FFIR';
	static const unsigned int CHUNK_FMT_TAG  = ' tmf';
	static const unsigned int CHUNK_DATA_TAG = 'atad';

	// RIFF�`�����N�͒ʏ�̃`�����N���Ƀt�H�[�}�b�g��񂪂�
	struct RIFFHeader
	{
		unsigned int tag;
		unsigned int size;
		unsigned int format;
	};

	// Wave�t�@�C���̃`�����N�����i�[���邽�߂̌^
	struct WaveChunk
	{
		unsigned int tag;
		unsigned int size;
	};

	ALuint		buffer_id = 0;
	ALuint		source_id = 0;

	FILE* fp = nullptr;
	WAVEFORMATEX format;
	long fileSize = -1;
	long dataSize = -1;

	static XMFLOAT3 listenerPos;

public:
	static constexpr float	SIDE_MAX = 120;
	static constexpr float	FRONT_MAX = 1000;

public:
	Audio();
	Audio(const char* path);
	virtual ~Audio();

private:
	void	Initialize();
	void	Release();
	bool	Open(const char* path);
	bool	Close();
	int		Read(void* dest, int length);

	int		parseHeader();
	bool	parseWaveChunk(WaveChunk* chunk, unsigned int tag);

public:
//	void playing()
//	{
//		ALint source_state;
//		alGetSourcei(source_id, AL_SOURCE_STATE, &source_state);
//		switch (source_state)
//		{
//		case AL_INITIAL:
//		case AL_PLAYING:
//			break;
//		case AL_PAUSED:
//		case AL_STOPPED:
//			Play(false);
//		}
//	}
//	aroundStageSound[SoundType::Rain]->playing();

	// �Đ�
	void	Play(const bool _isLoop)
	{
		alSourcei(source_id, AL_LOOPING, _isLoop ? AL_TRUE : AL_FALSE);
		alSourcePlay(source_id);
	}
	// �ꎞ��~
	void	Pause()
	{
		alSourcePause(source_id);
	}
	// ��~(����Đ����ŏ�����)
	void	Stop()
	{
		alSourceStop(source_id);
	}
	// 
	void	Rewind()
	{
		alSourceRewind(source_id);
	}
	// �����̍��W�Z�b�g		//���ς����恙
	void	SetPosition(const XMFLOAT3 _pos)
	{
		float DIST_MAX = sqrtf(SIDE_MAX * SIDE_MAX + FRONT_MAX * FRONT_MAX);
		ALfloat pos[] = {
			_pos.x - listenerPos.x,		// x
			0.0f,						// y
			_pos.z - listenerPos.z		// z
		};

		float distance = sqrtf(pos[0] * pos[0] + pos[2] * pos[2]);
		float volume = 1 - (distance / DIST_MAX);

		pos[0] = pos[0] / SIDE_MAX;
		pos[2] = pos[2] / FRONT_MAX;

 		SetVolume(volume);
		alSourcefv(source_id, AL_POSITION, pos);
	}
	void	SetVolume(const float volume)
	{
		alSourcef(source_id, AL_GAIN, volume);
	}

	// listener�̍��W�Z�b�g
	void	SetListenerPos(const XMFLOAT3 _pos)
	{
		listenerPos = _pos;
		listenerPos.y = 0.0f;
	}
};

long GetFileSize(FILE* fp);

#endif	// !_WAVEFILE_H_