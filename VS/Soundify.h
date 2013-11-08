#pragma once
#include "AL/al.h"
#include "AL/alc.h"
#pragma comment(lib, "libOpenAL32.dll.a")

class Soundify
{
public:
	Soundify();
	~Soundify();
	void Play();
	void SetPitch( float val );
	void SetGain( float val );

private:
	void GenerateAudioData(double frequency, int seconds);
	ALboolean CheckALCError();
	ALboolean CheckALError();

	ALCdevice* pDevice;
	ALCcontext* pContext;
	short* audioData;
	ALuint buffer;
	ALuint source;
	int frames;
	int sampleRate;
};

