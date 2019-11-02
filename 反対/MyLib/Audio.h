#ifndef _AUDIO_H_
#define _AUDIO_H_
#include <xaudio2.h>
#include "WAVFileReader.h"
#include <map>
#include <memory>
#include <vector>

#define NEW

#ifndef NEW
class Audio
{
private:
	static int id;
	IXAudio2* pXAudio2 = nullptr;
	IXAudio2MasteringVoice* pMasteringVoice;
	std::unique_ptr<uint8_t[]> waveFile;
	static std::map<int, IXAudio2SourceVoice*> sourceVoiceManager;

public:
	Audio();
	~Audio() { pXAudio2 = nullptr; pMasteringVoice = nullptr; }

	void Initialize();
	void Update();
	void Uninit();

	int	LoadSoundFile(IXAudio2* _pXaudio2, LPCWSTR _fileName);
	bool	PlaySound(int _id);
	HRESULT PlayWave(IXAudio2* pXaudio2, LPCWSTR szFilename);

	static Audio* GetInstance()
	{
		static Audio instance;
		return &instance;
	}

public:
	IXAudio2* GetIXAudio2() { return pXAudio2; }
};

#define pAudio	Audio::GetInstance()
#endif

#ifdef NEW
class AudioHandle
{
public:
	std::unique_ptr<uint8_t[]> waveFile;
	DirectX::WAVData waveData;
	std::vector<IXAudio2SourceVoice*> sourceVoice;

public:
	AudioHandle(){}
};


class Audio
{
	IXAudio2* pXAudio2;
	IXAudio2MasteringVoice* pMasteringVoice;



public:
	Audio();
	void Initialize();
	void Uninitialize();
	void Update(AudioHandle* _handle);

	void LoadSoundFile(LPCWSTR _fileName, AudioHandle* _handle);
	void Play(AudioHandle* _handle, bool _isLoop = false);
	void Stop(AudioHandle* _handle, int _arrayNum = 0);
	void SetVolume(AudioHandle* _handle, float volume, int _arrayNum = 0);
	void DeleteSourceVoice(AudioHandle* _handle);

	static Audio* GetInstance()
	{
		static Audio instance;
		return &instance;
	}
};
#define pAudio Audio::GetInstance() 


#endif //NEW

#endif	//_AUDIO_H_