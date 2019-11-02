#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <stdio.h>
#include "Audio.h"
#include <assert.h>

/*******************************************************************************
	Header File included.
*******************************************************************************/
#if(_WIN32_WINNT>=0x0602/*_WIN32_WINNT_WIN8*/)
#include <xaudio2.h>
#include "misc.h"

#include "WAVFileReader.h"

#pragma comment(lib,"xaudio2.lib")
#else
#include <C:\Program Files (x86)\Microsoft DirectX SDK (June 2010)\Include\comdecl.h>
#include <C:\Program Files (x86)\Microsoft DirectX SDK (June 2010)\Include\xaudio2.h>
#endif

#ifndef SAFE_RELEASE
#define SAFE_RELEASE(p)      { if(p) { (p)->Release(); (p)=nullptr; } }
#endif


HRESULT FindMediaFileCch(WCHAR* strDestPath, int cchDest, LPCWSTR strFilename);
#ifndef NEW
/*******************************************************************************
	プロトタイプ宣言
*******************************************************************************/

HRESULT FindMediaFileCch(WCHAR* strDestPath, int cchDest, LPCWSTR strFilename);
std::map<int, IXAudio2SourceVoice*> Audio::sourceVoiceManager;
int Audio::id;
/*******************************************************************************
	Functions
*******************************************************************************/
Audio::Audio()
{
	pXAudio2 = nullptr;
	pMasteringVoice = nullptr;
	id = 0;
}

void Audio::Initialize()
{
	HRESULT hr;
	pXAudio2 = nullptr;

	//これはXAudio2を使用する前のおまじないです。
	//This is Spell before I use XAudio2.
	CoInitializeEx(nullptr, COINIT_MULTITHREADED);

	//XAudio2の初期化
	//XAudio2 Initialze.

#if 1
	UINT32 flags = 0;
#if(_WIN32_WINNT < 0x0602/*_WIN32_WINNT_WIN8*/ && defined(_DEBUG))
	flags |= XAUDIO2_DEBUG_ENGINE;			//デバッグフラグ立てる
#endif

	hr = XAudio2Create(&pXAudio2, flags);		//出でよシェ◯ロン
	if (FAILED(hr))
	{
		CoUninitialize();
		assert(SUCCEEDED(hr) && "Audio.cpp Line: 35");
	}
#else
	IXAudio2* pXAudio2_ = nullptr;

	UINT32 flags = 0;
#if (_WIN32_WINNT < 0x0602 /*_WIN32_WINNT_WIN8*/) && defined(_DEBUG)
	flags |= XAUDIO2_DEBUG_ENGINE;
#endif
	hr = XAudio2Create(&pXAudio2_, flags);
	if (FAILED(hr))
	{
		CoUninitialize();
		assert(SUCCEEDED(hr) && "Audio.cpp Line: 35");
	}
#endif

	//Create Mastering Voice
	IXAudio2MasteringVoice* pMasteringVoice = nullptr;
	hr = pXAudio2->CreateMasteringVoice(&pMasteringVoice);
	if (FAILED(hr))
	{
		SAFE_RELEASE(pXAudio2);
		CoUninitialize();
		assert(SUCCEEDED(hr) && "Audio.cpp Line: 44");
	}
}

void Audio::Update()
{
	if (GetAsyncKeyState('P') & 1)
	{
		PlaySound(0);
	}
	if (GetAsyncKeyState('D') & 1)
	{
		Uninit();
	}
}

void Audio::Uninit()
{
	//Destroy MasteringVoice
	pMasteringVoice->DestroyVoice();

	//Destroy XAudio2
	SAFE_RELEASE(pXAudio2);

	//Destroy COM
	CoUninitialize();
}



int Audio::LoadSoundFile(IXAudio2* _pXaudio2, LPCWSTR _fileName)
{
	//
	// Locate the wave file
	//
	WCHAR strFilePath[MAX_PATH];
	HRESULT hr = FindMediaFileCch(strFilePath, MAX_PATH, _fileName);
	if (FAILED(hr))
	{
		wprintf(L"Failed to find media file: %s\n", _fileName);
		return -1;
	}

	//
	// Read in the wave file
	//
	DirectX::WAVData waveData;
	if (FAILED(hr = DirectX::LoadWAVAudioFromFileEx(strFilePath, waveFile, waveData)))
	{
		wprintf(L"Failed reading WAV file: %#X (%s)\n", hr, strFilePath);
		return -1;
	}

	//
	// Play the wave using a XAudio2SourceVoice
	//

	// Create the source voice
	IXAudio2SourceVoice* pSourceVoice;
	if (FAILED(hr = _pXaudio2->CreateSourceVoice(&pSourceVoice, waveData.wfx)))
	{
		wprintf(L"Error %#X creating source voice\n", hr);
		return -1;
	}

	// Submit the wave sample data using an XAUDIO2_BUFFER structure
	XAUDIO2_BUFFER buffer = { 0 };
	buffer.pAudioData = waveData.startAudio;
	buffer.Flags = XAUDIO2_END_OF_STREAM;  // tell the source voice not to expect any data after this buffer
	buffer.AudioBytes = waveData.audioBytes;

	if (waveData.loopLength > 0)
	{
		buffer.LoopBegin = waveData.loopStart;
		buffer.LoopLength = waveData.loopLength;
		buffer.LoopCount = 1; // We'll just assume we play the loop twice
	}

#if (_WIN32_WINNT < 0x0602 /*_WIN32_WINNT_WIN8*/)
	if (waveData.seek)
	{
		XAUDIO2_BUFFER_WMA xwmaBuffer = { 0 };
		xwmaBuffer.pDecodedPacketCumulativeBytes = waveData.seek;
		xwmaBuffer.PacketCount = waveData.seekCount;
		if (FAILED(hr = pSourceVoice->SubmitSourceBuffer(&buffer, &xwmaBuffer)))
		{
			wprintf(L"Error %#X submitting source buffer (xWMA)\n", hr);
			pSourceVoice->DestroyVoice();
			return hr;
		}
	}
#else
	if (waveData.seek)
	{
		wprintf(L"This platform does not support xWMA or XMA2\n");
		pSourceVoice->DestroyVoice();
		return -1;
	}
#endif
	else if (FAILED(hr = pSourceVoice->SubmitSourceBuffer(&buffer)))
	{
		wprintf(L"Error %#X submitting source buffer\n", hr);
		pSourceVoice->DestroyVoice();
		return -1;
	}
	sourceVoiceManager.insert(std::make_pair(id, pSourceVoice));

	return id++;
}


bool Audio::PlaySound(int _id)
{
	IXAudio2SourceVoice* source = sourceVoiceManager.at(_id);
	if (!source)
	{
		return false;
	}

	HRESULT hr = source->Start(0);
	if (FAILED(hr))
	{
		return false;
	}
	return true;
}

_Use_decl_annotations_
HRESULT Audio::PlayWave(IXAudio2* pXaudio2, LPCWSTR szFilename)
{

	//
	// Locate the wave file
	//
	WCHAR strFilePath[MAX_PATH];
	HRESULT hr = FindMediaFileCch(strFilePath, MAX_PATH, szFilename);
	if (FAILED(hr))
	{
		wprintf(L"Failed to find media file: %s\n", szFilename);
		return hr;
	}

	//
	// Read in the wave file
	//
	DirectX::WAVData waveData;
	if (FAILED(hr = DirectX::LoadWAVAudioFromFileEx(strFilePath, waveFile, waveData)))
	{
		wprintf(L"Failed reading WAV file: %#X (%s)\n", hr, strFilePath);
		return hr;
	}

	//
	// Play the wave using a XAudio2SourceVoice
	//

	// Create the source voice
	IXAudio2SourceVoice* pSourceVoice;
	if (FAILED(hr = pXaudio2->CreateSourceVoice(&pSourceVoice, waveData.wfx)))
	{
		wprintf(L"Error %#X creating source voice\n", hr);
		return hr;
	}

	// Submit the wave sample data using an XAUDIO2_BUFFER structure
	XAUDIO2_BUFFER buffer = { 0 };
	buffer.pAudioData = waveData.startAudio;
	buffer.Flags = XAUDIO2_END_OF_STREAM;  // tell the source voice not to expect any data after this buffer
	buffer.AudioBytes = waveData.audioBytes;

	if (waveData.loopLength > 0)
	{
		buffer.LoopBegin = waveData.loopStart;
		buffer.LoopLength = waveData.loopLength;
		buffer.LoopCount = 1; // We'll just assume we play the loop twice
	}

#if (_WIN32_WINNT < 0x0602 /*_WIN32_WINNT_WIN8*/)
	if (waveData.seek)
	{
		XAUDIO2_BUFFER_WMA xwmaBuffer = { 0 };
		xwmaBuffer.pDecodedPacketCumulativeBytes = waveData.seek;
		xwmaBuffer.PacketCount = waveData.seekCount;
		if (FAILED(hr = pSourceVoice->SubmitSourceBuffer(&buffer, &xwmaBuffer)))
		{
			wprintf(L"Error %#X submitting source buffer (xWMA)\n", hr);
			pSourceVoice->DestroyVoice();
			return hr;
		}
	}
#else
	if (waveData.seek)
	{
		wprintf(L"This platform does not support xWMA or XMA2\n");
		pSourceVoice->DestroyVoice();
		return hr;
	}
#endif
	else if (FAILED(hr = pSourceVoice->SubmitSourceBuffer(&buffer)))
	{
		wprintf(L"Error %#X submitting source buffer\n", hr);
		pSourceVoice->DestroyVoice();
		return hr;
	}


	//再生関数に持っていく---------
	hr = pSourceVoice->Start(0);
	//---------------------------

	// Let the sound play
	BOOL isRunning = TRUE;
	while (SUCCEEDED(hr) && isRunning)
	{
		XAUDIO2_VOICE_STATE state;
		pSourceVoice->GetState(&state);
		isRunning = (state.BuffersQueued > 0) != 0;

		// Wait till the escape key is pressed
		if (GetAsyncKeyState('S'))
			break;

		Sleep(10);
	}

	// Wait till the escape key is released
	while (GetAsyncKeyState('S'))
		Sleep(10);

	pSourceVoice->DestroyVoice();

	return hr;

}


//--------------------------------------------------------------------------------------
// Helper function to try to find the location of a media file
//--------------------------------------------------------------------------------------
_Use_decl_annotations_
HRESULT FindMediaFileCch(WCHAR* strDestPath, int cchDest, LPCWSTR strFilename)
{
	bool bFound = false;

	if (!strFilename || strFilename[0] == 0 || !strDestPath || cchDest < 10)
		return E_INVALIDARG;

	// Get the exe name, and exe path
	WCHAR strExePath[MAX_PATH] = { 0 };
	WCHAR strExeName[MAX_PATH] = { 0 };
	WCHAR* strLastSlash = nullptr;
	GetModuleFileName(nullptr, strExePath, MAX_PATH);
	strExePath[MAX_PATH - 1] = 0;
	strLastSlash = wcsrchr(strExePath, TEXT('\\'));
	if (strLastSlash)
	{
		wcscpy_s(strExeName, MAX_PATH, &strLastSlash[1]);

		// Chop the exe name from the exe path
		*strLastSlash = 0;

		// Chop the .exe from the exe name
		strLastSlash = wcsrchr(strExeName, TEXT('.'));
		if (strLastSlash)
			*strLastSlash = 0;
	}

	wcscpy_s(strDestPath, cchDest, strFilename);
	if (GetFileAttributes(strDestPath) != 0xFFFFFFFF)
		return S_OK;

	// Search all parent directories starting at .\ and using strFilename as the leaf name
	WCHAR strLeafName[MAX_PATH] = { 0 };
	wcscpy_s(strLeafName, MAX_PATH, strFilename);

	WCHAR strFullPath[MAX_PATH] = { 0 };
	WCHAR strFullFileName[MAX_PATH] = { 0 };
	WCHAR strSearch[MAX_PATH] = { 0 };
	WCHAR* strFilePart = nullptr;

	GetFullPathName(L".", MAX_PATH, strFullPath, &strFilePart);
	if (!strFilePart)
		return E_FAIL;

	while (strFilePart && *strFilePart != '\0')
	{
		swprintf_s(strFullFileName, MAX_PATH, L"%s\\%s", strFullPath, strLeafName);
		if (GetFileAttributes(strFullFileName) != 0xFFFFFFFF)
		{
			wcscpy_s(strDestPath, cchDest, strFullFileName);
			bFound = true;
			break;
		}

		swprintf_s(strFullFileName, MAX_PATH, L"%s\\%s\\%s", strFullPath, strExeName, strLeafName);
		if (GetFileAttributes(strFullFileName) != 0xFFFFFFFF)
		{
			wcscpy_s(strDestPath, cchDest, strFullFileName);
			bFound = true;
			break;
		}

		swprintf_s(strSearch, MAX_PATH, L"%s\\..", strFullPath);
		GetFullPathName(strSearch, MAX_PATH, strFullPath, &strFilePart);
	}
	if (bFound)
		return S_OK;

	// On failure, return the file as the path but also return an error code
	wcscpy_s(strDestPath, cchDest, strFilename);

	return HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND);
}
#endif

#ifdef NEW

Audio::Audio()
{
	pXAudio2 = nullptr;
	pMasteringVoice = nullptr;
}

void Audio::Initialize()
{
	HRESULT hr;
	pXAudio2 = nullptr;

	//これはXAudio2を使用する前のおまじないです。
	//This is Spell before I use XAudio2.
	CoInitializeEx(nullptr, COINIT_MULTITHREADED);

	//XAudio2の初期化
	//XAudio2 Initialze.
#if 1
	UINT32 flags = 0;
#if(_WIN32_WINNT < 0x0602/*_WIN32_WINNT_WIN8*/ && defined(_DEBUG))
	flags |= XAUDIO2_DEBUG_ENGINE;			//デバッグフラグ立てる
#endif

	hr = XAudio2Create(&pXAudio2, flags);		//出でよシェ◯ロン
	if (FAILED(hr))
	{
		CoUninitialize();
		assert(SUCCEEDED(hr) && "Audio.cpp Line: 35");
	}
#else
	IXAudio2* pXAudio2_ = nullptr;

	UINT32 flags = 0;
#if (_WIN32_WINNT < 0x0602 /*_WIN32_WINNT_WIN8*/) && defined(_DEBUG)
	flags |= XAUDIO2_DEBUG_ENGINE;
#endif
	hr = XAudio2Create(&pXAudio2_, flags);
	if (FAILED(hr))
	{
		CoUninitialize();
		assert(SUCCEEDED(hr) && "Audio.cpp Line: 35");
	}
#endif

	//Create Mastering Voice
	pMasteringVoice = nullptr;
	hr = pXAudio2->CreateMasteringVoice(&pMasteringVoice);
	if (FAILED(hr))
	{
		SAFE_RELEASE(pXAudio2);
		CoUninitialize();
		assert(SUCCEEDED(hr) && "Audio.cpp Line: 44");
	}
}

void Audio::Update(AudioHandle* _handle)
{
	//size_t size = _handle->sourceVoice.size();
	for (int i = 0; i < (int)_handle->sourceVoice.size(); i++)
	{
		bool isRunning = true;
		XAUDIO2_VOICE_STATE state;
		_handle->sourceVoice[i]->GetState(&state);

		isRunning = (state.BuffersQueued > 0) != 0;
		if (!isRunning)
		{
			Stop(_handle, i);
			_handle->sourceVoice[i]->DestroyVoice();
			_handle->sourceVoice.erase(_handle->sourceVoice.begin() + i);
		}
	}
}

void Audio::Uninitialize()
{
	//Destroy MasteringVoice
	pMasteringVoice->DestroyVoice();

	//Destroy XAudio2
	SAFE_RELEASE(pXAudio2);

	//Destroy COM
	CoUninitialize();
}


void Audio::LoadSoundFile(LPCWSTR _fileName, AudioHandle* _handle)
{
	//
	// Locate the wave file
	//
	WCHAR strFilePath[MAX_PATH];
	HRESULT hr = FindMediaFileCch(strFilePath, MAX_PATH, _fileName);
	if (FAILED(hr))
	{
		wprintf(L"Failed to find media file: %s\n", _fileName);
		assert(SUCCEEDED(hr) && "Audio.cpp Line:481");
	}

	//
	// Read in the wave file
	//
//	DirectX::WAVData waveData;
	if (FAILED(hr = DirectX::LoadWAVAudioFromFileEx(strFilePath, _handle->waveFile, _handle->waveData)))
	{
		wprintf(L"Failed reading WAV file: %#X (%s)\n", hr, strFilePath);
		assert(SUCCEEDED(hr) && "Audio.cpp Line:492");
	}


}

#if 0
_Use_decl_annotations_
HRESULT Audio::PlayWave(IXAudio2* pXaudio2, LPCWSTR szFilename)
{

	//
	// Locate the wave file
	//
	WCHAR strFilePath[MAX_PATH];
	HRESULT hr = FindMediaFileCch(strFilePath, MAX_PATH, szFilename);
	if (FAILED(hr))
	{
		wprintf(L"Failed to find media file: %s\n", szFilename);
		return hr;
	}

	//
	// Read in the wave file
	//
	DirectX::WAVData waveData;
	if (FAILED(hr = DirectX::LoadWAVAudioFromFileEx(strFilePath, waveFile, waveData)))
	{
		wprintf(L"Failed reading WAV file: %#X (%s)\n", hr, strFilePath);
		return hr;
	}

	//
	// Play the wave using a XAudio2SourceVoice
	//

	// Create the source voice
	IXAudio2SourceVoice* pSourceVoice;
	if (FAILED(hr = pXaudio2->CreateSourceVoice(&pSourceVoice, waveData.wfx)))
	{
		wprintf(L"Error %#X creating source voice\n", hr);
		return hr;
	}

	// Submit the wave sample data using an XAUDIO2_BUFFER structure
	XAUDIO2_BUFFER buffer = { 0 };
	buffer.pAudioData = waveData.startAudio;
	buffer.Flags = XAUDIO2_END_OF_STREAM;  // tell the source voice not to expect any data after this buffer
	buffer.AudioBytes = waveData.audioBytes;

	if (waveData.loopLength > 0)
	{
		buffer.LoopBegin = waveData.loopStart;
		buffer.LoopLength = waveData.loopLength;
		buffer.LoopCount = 1; // We'll just assume we play the loop twice
	}

#if (_WIN32_WINNT < 0x0602 /*_WIN32_WINNT_WIN8*/)
	if (waveData.seek)
	{
		XAUDIO2_BUFFER_WMA xwmaBuffer = { 0 };
		xwmaBuffer.pDecodedPacketCumulativeBytes = waveData.seek;
		xwmaBuffer.PacketCount = waveData.seekCount;
		if (FAILED(hr = pSourceVoice->SubmitSourceBuffer(&buffer, &xwmaBuffer)))
		{
			wprintf(L"Error %#X submitting source buffer (xWMA)\n", hr);
			pSourceVoice->DestroyVoice();
			return hr;
		}
	}
#else
	if (waveData.seek)
	{
		wprintf(L"This platform does not support xWMA or XMA2\n");
		pSourceVoice->DestroyVoice();
		return hr;
	}
#endif
	else if (FAILED(hr = pSourceVoice->SubmitSourceBuffer(&buffer)))
	{
		wprintf(L"Error %#X submitting source buffer\n", hr);
		pSourceVoice->DestroyVoice();
		return hr;
	}


	//再生関数に持っていく---------
	hr = pSourceVoice->Start(0);
	//---------------------------

	// Let the sound play
	BOOL isRunning = TRUE;
	while (SUCCEEDED(hr) && isRunning)
	{
		XAUDIO2_VOICE_STATE state;
		pSourceVoice->GetState(&state);
		isRunning = (state.BuffersQueued > 0) != 0;

		// Wait till the escape key is pressed
		if (GetAsyncKeyState('S'))
			break;

		Sleep(10);
	}

	// Wait till the escape key is released
	while (GetAsyncKeyState('S'))
		Sleep(10);

	pSourceVoice->DestroyVoice();

	return hr;

}
#endif

void Audio::Play(AudioHandle* _handle, bool _isLoop)
{
	HRESULT hr = S_OK;
	//
	// Play the wave using a XAudio2SourceVoice
	//

	// Create the source voice
	IXAudio2SourceVoice* pSourceVoice;
	if (FAILED(hr = pXAudio2->CreateSourceVoice(&pSourceVoice, _handle->waveData.wfx)))
	{
		wprintf(L"Error %#X creating source voice\n", hr);
		assert(SUCCEEDED(hr) && "Audio.cpp Line:504");
	}

	// Submit the wave sample data using an XAUDIO2_BUFFER structure
	XAUDIO2_BUFFER buffer = { 0 };
	buffer.pAudioData = _handle->waveData.startAudio;
	buffer.Flags = XAUDIO2_END_OF_STREAM;  // tell the source voice not to expect any data after this buffer
	buffer.AudioBytes = _handle->waveData.audioBytes;

	if (_handle->waveData.loopLength > 0)
	{
		buffer.LoopBegin = _handle->waveData.loopStart;
		buffer.LoopLength = _handle->waveData.loopLength;
		buffer.LoopCount = 1; // We'll just assume we play the loop twice
	}

#if (_WIN32_WINNT < 0x0602 /*_WIN32_WINNT_WIN8*/)
	if (waveData.seek)
	{
		XAUDIO2_BUFFER_WMA xwmaBuffer = { 0 };
		xwmaBuffer.pDecodedPacketCumulativeBytes = waveData.seek;
		xwmaBuffer.PacketCount = waveData.seekCount;
		if (FAILED(hr = pSourceVoice->SubmitSourceBuffer(&buffer, &xwmaBuffer)))
		{
			wprintf(L"Error %#X submitting source buffer (xWMA)\n", hr);
			pSourceVoice->DestroyVoice();
			return hr;
		}
	}
#else
	//ループするならこれ使う
	if (_isLoop)
	{
		buffer.LoopCount = XAUDIO2_LOOP_INFINITE;
	}


	if (_handle->waveData.seek)
	{
		wprintf(L"This platform does not support xWMA or XMA2\n");
		pSourceVoice->DestroyVoice();
		assert(_handle->waveData.seek != NULL && "Audio.cpp Line:537");
	}
#endif
	else if (FAILED(hr = pSourceVoice->SubmitSourceBuffer(&buffer)))
	{
		wprintf(L"Error %#X submitting source buffer\n", hr);
		pSourceVoice->DestroyVoice();
		assert(SUCCEEDED(hr) && "Audio.cpp Line:544");
	}

	//
	// Let's Play
	//
	pSourceVoice->Start(0);
	pSourceVoice->SetVolume(0.5f*0.5f);

	_handle->sourceVoice.push_back(pSourceVoice);

}

void Audio::Stop(AudioHandle* _handle, int _arrayNum )
{
	HRESULT hr = S_OK;

	hr = _handle->sourceVoice[_arrayNum]->Stop(XAUDIO2_PLAY_TAILS);
	assert(SUCCEEDED(hr) && " Audio.cpp Line : 704 ");
}

void Audio::SetVolume(AudioHandle* _handle, float volume, int arrayNum)
{
	HRESULT hr = S_OK;

	float vo = 0.0f;
	 _handle->sourceVoice[arrayNum]->GetVolume(&vo);
	hr = _handle->sourceVoice[arrayNum]->SetVolume(volume*volume);
	assert(SUCCEEDED(hr) && " Audio.cpp Line : 715 ");
}

void Audio::DeleteSourceVoice(AudioHandle* _handle)
{
	size_t size = _handle->sourceVoice.size();

	for (int i = 0; i < (int)size; i++)
	{
		_handle->sourceVoice[i]->DestroyVoice();
		_handle->sourceVoice.erase(_handle->sourceVoice.begin() + i);
	}
	//	_handle->sourceVoice.clear();
}

//--------------------------------------------------------------------------------------
// Helper function to try to find the location of a media file
//--------------------------------------------------------------------------------------
_Use_decl_annotations_
HRESULT FindMediaFileCch(WCHAR* strDestPath, int cchDest, LPCWSTR strFilename)
{
	bool bFound = false;

	if (!strFilename || strFilename[0] == 0 || !strDestPath || cchDest < 10)
		return E_INVALIDARG;

	// Get the exe name, and exe path
	WCHAR strExePath[MAX_PATH] = { 0 };
	WCHAR strExeName[MAX_PATH] = { 0 };
	WCHAR* strLastSlash = nullptr;
	GetModuleFileName(nullptr, strExePath, MAX_PATH);
	strExePath[MAX_PATH - 1] = 0;
	strLastSlash = wcsrchr(strExePath, TEXT('\\'));
	if (strLastSlash)
	{
		wcscpy_s(strExeName, MAX_PATH, &strLastSlash[1]);

		// Chop the exe name from the exe path
		*strLastSlash = 0;

		// Chop the .exe from the exe name
		strLastSlash = wcsrchr(strExeName, TEXT('.'));
		if (strLastSlash)
			*strLastSlash = 0;
	}

	wcscpy_s(strDestPath, cchDest, strFilename);
	if (GetFileAttributes(strDestPath) != 0xFFFFFFFF)
		return S_OK;

	// Search all parent directories starting at .\ and using strFilename as the leaf name
	WCHAR strLeafName[MAX_PATH] = { 0 };
	wcscpy_s(strLeafName, MAX_PATH, strFilename);

	WCHAR strFullPath[MAX_PATH] = { 0 };
	WCHAR strFullFileName[MAX_PATH] = { 0 };
	WCHAR strSearch[MAX_PATH] = { 0 };
	WCHAR* strFilePart = nullptr;

	GetFullPathName(L".", MAX_PATH, strFullPath, &strFilePart);
	if (!strFilePart)
		return E_FAIL;

	while (strFilePart && *strFilePart != '\0')
	{
		swprintf_s(strFullFileName, MAX_PATH, L"%s\\%s", strFullPath, strLeafName);
		if (GetFileAttributes(strFullFileName) != 0xFFFFFFFF)
		{
			wcscpy_s(strDestPath, cchDest, strFullFileName);
			bFound = true;
			break;
		}

		swprintf_s(strFullFileName, MAX_PATH, L"%s\\%s\\%s", strFullPath, strExeName, strLeafName);
		if (GetFileAttributes(strFullFileName) != 0xFFFFFFFF)
		{
			wcscpy_s(strDestPath, cchDest, strFullFileName);
			bFound = true;
			break;
		}

		swprintf_s(strSearch, MAX_PATH, L"%s\\..", strFullPath);
		GetFullPathName(strSearch, MAX_PATH, strFullPath, &strFilePart);
	}
	if (bFound)
		return S_OK;

	// On failure, return the file as the path but also return an error code
	wcscpy_s(strDestPath, cchDest, strFilename);

	return HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND);
}

#endif