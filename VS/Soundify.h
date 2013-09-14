#pragma once
#include "AL/al.h"
#include "AL/alc.h"
#ifdef WIN32
#pragma comment(lib, "libOpenAL32.dll.a")
#endif



/**
 * Audio sample rate
 */
#ifndef AUDIO_SAMPLE_RATE
#define AUDIO_SAMPLE_RATE  44100
#endif

/**
 * Audio frequency, in hertz
 */
#ifndef AUDIO_FREQUENCY
#define AUDIO_FREQUENCY  440
#endif

/**
 * Audio duration, in milliseconds
 */
#ifndef AUDIO_DURATION
#define AUDIO_DURATION  1000
#endif



/**
 * Initialise the audio emitted when sculpting
 */
void initialise_audio();

/**
 * Uninitialise the audio
 */
void terminate_audio();


/**
 * Set the audio playback pitch
 * 
 * @param  pitch  Frequency mulitplayer
 */
void audio_set_pitch(float pitch);

/**
 * Set the audio playback gain
 * 
 * @param  gain  Volume mulitplayer
 */
void audio_set_gain(float gain);

