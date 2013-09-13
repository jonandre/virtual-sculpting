#include "Soundify.h"
#include "main.h"


Soundify::Soundify()
{
	sampleRate = 44100;
	GenerateAudioData( 1000, 1 );
	pDevice = alcOpenDevice(NULL);
	CheckALCError();
	pContext = alcCreateContext(pDevice, NULL);
	CheckALCError();

	alcMakeContextCurrent(pContext);
	CheckALCError();
	alGenBuffers(1, &buffer);
	CheckALError();
	alGenSources(1, &source);
	CheckALError();

	alBufferData(buffer, AL_FORMAT_MONO16, audioData, frames, sampleRate);
	CheckALError();
	alSourcei(source, AL_BUFFER, buffer);
	alSourcef(source, AL_PITCH,    1.0f);
    alSourcef(source, AL_GAIN,    0.0f);
	alSourcei(source,AL_LOOPING,AL_TRUE);
}

void Soundify::SetPitch( float val )
{
	alSourcef(source, AL_PITCH, val);
}


void Soundify::SetGain( float val )
{
	alSourcef(source, AL_GAIN, val);
}


void Soundify::Play()
{
	alSourcePlay(source);
}

void Soundify::GenerateAudioData(double frequency, int seconds)
{	
	frames = seconds * sampleRate;
	audioData = new short[frames];
	unsigned int MaxValue = 1<<(sizeof(short)*8 - 1) - 1;
	for (int i = 0; i < frames; i++)
	{
		audioData[i] = (short)(MaxValue * sin((2 * glm::pi<double>() * frequency) / sampleRate * i));
	}
}


Soundify::~Soundify()
{
	delete []audioData;

	alDeleteBuffers(1, &buffer);
	alDeleteSources(1, &source);
	
	alcMakeContextCurrent(NULL);
	alcDestroyContext(pContext);
	alcCloseDevice(pDevice);
}


ALboolean Soundify::CheckALCError()
{
  ALenum ErrCode;
  string Err = "ALC error: ";
  if ((ErrCode = alcGetError(pDevice)) != ALC_NO_ERROR)
  {
    Err += (char *)alcGetString(pDevice, ErrCode);
	std::cout<<Err.data()<<std::endl;
    return AL_FALSE;
  }
  return AL_TRUE;
}

ALboolean Soundify::CheckALError()
{
  ALenum ErrCode;
  string Err = "OpenAL error: ";
  if ((ErrCode = alGetError()) != AL_NO_ERROR)
  {
    Err += (char *)alGetString(ErrCode);
    std::cout<<Err.data()<<std::endl;
    return AL_FALSE;
  }
  return AL_TRUE;
}