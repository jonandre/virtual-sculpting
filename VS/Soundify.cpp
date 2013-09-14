#include "Soundify.h"
#include "main.h"



#define FRAMES     (AUDIO_DURATION * AUDIO_SAMPLE_RATE / 1000)
#define MAX_VALUE  ((1 << (sizeof(short) * 8 - 1)) - 1)

static short audioData[FRAMES];
static ALCdevice* audioDevice;
static ALCcontext* audioContext;

static ALuint audioBuffer;
static ALuint audioSource;



/**
 * Check for audio device error
 */
inline void check_alc_error()
{
	ALenum err;
	if ((err = alcGetError(audioDevice)) != ALC_NO_ERROR)
		std::cerr << "ALC error: " << alcGetString(audioDevice, err) << std::endl;
}

/**
 * Check for audio error
 */
inline void check_al_error()
{
	ALenum err;
	if ((err = alGetError()) != AL_NO_ERROR)
		std::cerr << "OpenAL error: " << alGetString(err) << std::endl;
}


/**
 * Initialise the audio emitted when sculpting
 */
void initialise_audio()
{
	/* Generate wave to play */
	for (int i = 0; i < FRAMES; i++) /* TODO add overtones for a cleaner sound */
		audioData[i] = (short)(MAX_VALUE * sin((2 * glm::pi<double>() * AUDIO_FREQUENCY) / AUDIO_SAMPLE_RATE * i));
	
	/* Open default audio playback device */
	audioDevice = alcOpenDevice(NULL);                   check_alc_error();
	
	/* Create audio playback context */
	audioContext = alcCreateContext(audioDevice, NULL);  check_alc_error();
	alcMakeContextCurrent(audioContext);                 check_alc_error();
	
	/* Generate buffer and source */
	alGenBuffers(1, &audioBuffer);                       check_al_error();
	alGenSources(1, &audioSource);                       check_al_error();
	
	/* Configure audio playback */
	alBufferData(audioBuffer, AL_FORMAT_MONO16, audioData, FRAMES, AUDIO_SAMPLE_RATE);
	check_al_error();
	alSourcei(audioSource, AL_BUFFER,  audioBuffer);
	alSourcef(audioSource, AL_PITCH,   1.0f);
	alSourcef(audioSource, AL_GAIN,    0.0f);
	alSourcei(audioSource, AL_LOOPING, AL_TRUE);
	
	/* Start playback */
	alSourcePlay(audioSource);
}


/**
 * Uninitialise the audio
 */
void terminate_audio()
{
	audio_set_gain(0.0f);
	
	alDeleteBuffers(1, &audioBuffer);
	alDeleteSources(1, &audioSource);
	
	alcMakeContextCurrent(NULL);
	alcDestroyContext(audioContext);
	alcCloseDevice(audioDevice);
}


/**
 * Set the audio playback pitch
 * 
 * @param  pitch  Frequency mulitplayer
 */
void audio_set_pitch(float pitch)
{
	alSourcef(audioSource, AL_PITCH, pitch);
}


/**
 * Set the audio playback gain
 * 
 * @param  gain  Volume mulitplayer
 */
void audio_set_gain(float gain)
{
	alSourcef(audioSource, AL_GAIN, gain);
}

