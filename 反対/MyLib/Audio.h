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
	// �Đ�
	void	Play(const bool _isLoop)
	{
		alSourcei(source_id, AL_PLAYING, _isLoop);
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
	// �����̍��W�Z�b�g
	void	SetPosition(const XMFLOAT3 _pos)
	{

		ALfloat pos[] = { _pos.x, 0.0f, _pos.z };
		alSourcefv(source_id, AL_POSITION, pos);
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