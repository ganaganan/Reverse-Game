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
	// チャンクを識別する為の値
	static const unsigned int CHUNK_RIFF_TAG = 'FFIR';
	static const unsigned int CHUNK_FMT_TAG  = ' tmf';
	static const unsigned int CHUNK_DATA_TAG = 'atad';

	// RIFFチャンクは通常のチャンク情報にフォーマット情報がつく
	struct RIFFHeader
	{
		unsigned int tag;
		unsigned int size;
		unsigned int format;
	};

	// Waveファイルのチャンク情報を格納するための型
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
	// 再生
	void	Play(const bool _isLoop)
	{
		alSourcei(source_id, AL_PLAYING, _isLoop);
		alSourcePlay(source_id);
	}
	// 一時停止
	void	Pause()
	{
		alSourcePause(source_id);
	}
	// 停止(次回再生時最初から)
	void	Stop()
	{
		alSourceStop(source_id);
	}
	// 音源の座標セット
	void	SetPosition(const XMFLOAT3 _pos)
	{

		ALfloat pos[] = { _pos.x, 0.0f, _pos.z };
		alSourcefv(source_id, AL_POSITION, pos);
	}

	// listenerの座標セット
	void	SetListenerPos(const XMFLOAT3 _pos)
	{
		listenerPos = _pos;
		listenerPos.y = 0.0f;
	}
};

long GetFileSize(FILE* fp);

#endif	// !_WAVEFILE_H_