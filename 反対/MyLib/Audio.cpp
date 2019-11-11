#include	"Audio.h"

Audio::Audio()
{

}


Audio::Audio(const char* path)
{
	Open(path);
	Initialize();
	Close();
}


Audio::~Audio()
{
	Release();
	buffer_id = 0;
	source_id = 0;
}


bool Audio::Open(const char* path)
{
	errno_t no = fopen_s(&fp, path, "rb");

	if (no)	return false;

	fileSize = GetFileSize(fp);
	parseHeader();
	return true;
}


bool Audio::Close()
{
	if (fp)
	{
		fclose(fp);
	}
	return true;
}


int Audio::Read(void* dest, int length)
{
	return fread(dest, 1, length, fp);
}


int Audio::parseHeader()
{
	RIFFHeader riffGeader;
	fread(&riffGeader, sizeof(RIFFHeader), 1, fp);
	if (riffGeader.tag != CHUNK_RIFF_TAG)
	{
		return 1;
	}

	// ファイルの先頭からのバイト数を保存
	long pos = ftell(fp);

	WaveChunk waveChunk;
	if (parseWaveChunk(&waveChunk, CHUNK_FMT_TAG))
	{
		fread(&format, sizeof(WAVEFORMATEX), 1, fp);
	}
	else
	{
		return 2;
	}

	// ファイル位置指示子を保存していた場所に設定
	fseek(fp, pos, SEEK_SET);

//	waveChunk;
	if (parseWaveChunk(&waveChunk, CHUNK_DATA_TAG))
	{
		dataSize = waveChunk.size;
		return 0;
	}
	else
	{
		return 3;
	}
}


bool Audio::parseWaveChunk(WaveChunk* chunk, unsigned int tag)
{
	while (!feof(fp))
	{
		fread(chunk, sizeof(WaveChunk), 1, fp);

		if (chunk->tag == tag)
		{
			return	true;
		}
		else
		{
			fseek(fp, chunk->size, SEEK_CUR);
		}
	}

	return false;
}


void Audio::Initialize()
{
	// Read wavefile	----------------------------------------
	unsigned long memory = 0, length = 0;
	// a sample length
	memory = format.wBitsPerSample / 8;
	// How much to read
	length = dataSize / memory;

#if 0
	char* buffer = new char[dataSize];
	Read(buffer, dataSize);
#else
	signed short* buffer = new signed short[length];
	Read(buffer, length);
#endif


	// Create buffer	----------------------------------------
	alGenBuffers(1, &buffer_id);
	alGenSources(1, &source_id);


#if 0
	// Distinguish between monaural and stereo	----------------
	ALenum ms_fomat;

	// monaural
	if (format.nChannels == 1)
	{
		if (format.wBitsPerSample == 8)
		{
			ms_fomat = AL_FORMAT_MONO8;
		}
		else
		{
			ms_fomat = AL_FORMAT_MONO16;
		}
	}
	// stereo
	else
	{
		if (format.wBitsPerSample == 8)
		{
			ms_fomat = AL_FORMAT_STEREO8;
		}
		else
		{
			ms_fomat = AL_FORMAT_STEREO16;
		}
	}
#else
	// Convert stereo to monaural	----------------------------
	ALenum ms_fomat;
	if (format.nChannels == 2)
	{
		for (int i = 0; i < length; i += 2)
		{
			// Take the average value
			buffer[i / 2] = buffer[i] / 2 + buffer[i + 1] / 2;
		}
	}

	length = length / format.nChannels;
	if (format.wBitsPerSample == 8)
	{
		ms_fomat = AL_FORMAT_MONO8;
	}
	else
	{
		ms_fomat = AL_FORMAT_MONO16;
	}
#endif


	// Transfer waveform data to buffer and connect to source---
	alBufferData(buffer_id, ms_fomat, &buffer[0], length * sizeof(signed short), format.nSamplesPerSec);

	// Set audio buffer on source object
	alSourcei(source_id, AL_BUFFER, buffer_id);

	// et cetera
	float pitch = 1.0f;
	alSourcef(source_id, AL_PITCH, pitch);

	float volume = 1.0f;
	alSourcef(source_id, AL_GAIN, volume);
}


void Audio::Release()
{
	// 再生と停止しソースオブジェクトとオーディオバッファを破棄
	alSourceStop(source_id);
	alDeleteBuffers(1, &buffer_id);
	alDeleteSources(1, &source_id);
}


long GetFileSize(FILE * fp) {
	long fileSize = -1;
	if (!fp) { return fileSize; }

	long pos = ftell(fp);
	fseek(fp, 0, SEEK_END);
	fileSize = ftell(fp);
	fseek(fp, pos, SEEK_SET);

	return fileSize;
}
