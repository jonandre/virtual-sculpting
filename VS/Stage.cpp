#define __STAGE_CPP__
#include "Stage.h"

#include <Windows.h>
#include <iostream>
#include <MMSystem.h>


int stage;
int lastWord;
int space_pressed;
bool hapticsON;
bool soundON;
bool viewDrawStage;

bool reinitiate_acted,	right_acted,		left_acted,					up_acted,
	 down_acted,		clockwise_acted,	counter_clockwise_acted,	back_acted,
	 freez_acted,		clear_acted,		front_acted,				rear_acted,
	 top_acted,			bottom_acted,		camera_right_acted,			camera_left_acted, 
	 slower_acted,		faster_acted;

bool front_view, back_view, top_view, bottom_view, right_view, left_view, freez, clear_view;

float actedIndex[26];


bool hand_is_on;
bool hapticsConnected = false;

bool speechON = true;

int acted = 0;

Stage::Stage(void)
{
	stage			= 1;
	space_pressed	= false;
	hapticsON		= false;
	soundON			= true;
	viewDrawStage	= true;

	reinitiate_acted	= right_acted		= left_acted				= up_acted			=
	down_acted			= clockwise_acted	= counter_clockwise_acted	= back_acted		= 
	freez_acted			= clear_acted		= front_acted				= rear_acted		=
	top_acted			= bottom_acted		= camera_right_acted		= camera_left_acted = 
	slower_acted		= faster_acted		= 
	false;

	for (int i = 0; i < 26; i++)
		actedIndex[i] = 1.0f;

	lastWord = 0;
}
Stage::~Stage(void)
{
}
int GetStage(void)
{
	return stage;
}
void SetStage(int _stage)
{
	 stage = _stage;
}

bool GetPressedStage(void)
{
	return space_pressed;
}
void SetPressedStage(bool pressed)
{
	 space_pressed = pressed;
}

bool GetHapticsStage(void)
{
	return hapticsON;
}
void SetHapticsStage(bool ON)
{
	 hapticsON = ON;
}


bool GetSoundStage(void)
{
	return soundON;
}
void SetSoundStage(bool ON)
{
	 soundON = ON;
}

bool GetReinitiateStage(void)
{
	return reinitiate_acted;
}
void SetReinitiateStage(bool ON)
{
	 reinitiate_acted = ON;
}
bool GetRightStage(void)
{
	return right_acted;
}
void SetRightStage(bool ON)
{
	 right_acted = ON;
}
bool GetLeftStage(void)
{
	return left_acted;
}
void SetLeftStage(bool ON)
{
	 left_acted = ON;
}
bool GetUpStage(void)
{
	return up_acted;
}
void SetUpStage(bool ON)
{
	 up_acted = ON;
}
bool GetDownStage(void)
{
	return down_acted;
}
void SetDownStage(bool ON)
{
	 down_acted = ON;
}
bool GetClockwiseStage(void)
{
	return clockwise_acted;
}
void SetClockwiseStage(bool ON)
{
	 clockwise_acted = ON;
}
bool GetCounterClockwiseStage(void)
{
	return counter_clockwise_acted;
}
void SetCounterClockwiseStage(bool ON)
{
	 counter_clockwise_acted = ON;
}
bool GetBackStage(void)
{
	return back_acted;
}
void SetBackStage(bool ON)
{
	 back_acted = ON;
}
bool GetFreezStage(void)
{
	return freez_acted;
}
void SetFreezStage(bool ON)
{
	 freez_acted = ON;
}
bool GetClearStage(void)
{
	return clear_acted;
}
void SetClearStage(bool ON)
{
	 clear_acted = ON;
}
bool GetFrontStage(void)
{
	return front_acted;
}
void SetFrontStage(bool ON)
{
	 front_acted = ON;
}
bool GetRearStage(void)
{
	return rear_acted;
}
void SetRearStage(bool ON)
{
	 rear_acted = ON;
}
bool GetTopStage(void)
{
	return top_acted;
}
void SetTopStage(bool ON)
{
	 top_acted = ON;
}
bool GetBottomStage(void)
{
	return bottom_acted;
}
void SetBottomStage(bool ON)
{
	 bottom_acted = ON;
}
bool GetCameraRightStage(void)
{
	return camera_right_acted;
}
void SetCameraRightStage(bool ON)
{
	 camera_right_acted = ON;
}
bool GetCameraLeftStage(void)
{
	return camera_left_acted;
}
void SetCameraLeftStage(bool ON)
{
	 camera_left_acted = ON;
}

bool GetFrontViewStage(void)
{
	return front_view;
}
void SetFrontViewStage(bool ON)
{
	 front_view = ON;
}
bool GetRearViewStage(void)
{
	return back_view;
}
void SetRearViewStage(bool ON)
{
	 back_view = ON;
}
bool GetTopViewStage(void)
{
	return top_view;
}
void SetTopViewStage(bool ON)
{
	 top_view = ON;
}
bool GetBottomViewStage(void)
{
	return bottom_view;
}
void SetBottomViewStage(bool ON)
{
	 bottom_view = ON;
}
bool GetRightViewStage(void)
{
	return right_view;
}
void SetRightViewStage(bool ON)
{
	 right_view = ON;
}
bool GetLeftViewStage(void)
{
	return left_view;
}
void SetLeftViewStage(bool ON)
{
	 left_view = ON;
}
bool GetFreezViewStage(void)
{
	return freez;
}
void SetFreezViewStage(bool ON)
{
	 freez = ON;
}
bool GetclearViewStage(void)
{
	return clear_view;
}
void SetclearViewStage(bool ON)
{
	 clear_view = ON;
}

bool GetSlowerStage(void)
{
	return slower_acted;
}
void SetSlowerStage(bool ON)
{
	 slower_acted = ON;
}

bool GetFasterStage(void)
{
	return faster_acted;
}
void SetFasterStage(bool ON)
{
	 faster_acted = ON;
}




void hand_on()
{
	if (!hand_is_on)
	{
			//PlaySound(TEXT("VS-start-sound.wav"), NULL, SND_ASYNC); 
		/*
		std::cerr << "Send on" << std::endl;
		sendto(sock, hand_on_packet, 16, 0, (struct sockaddr*)&send_address, sizeof(send_address));
		std::cerr << "Send on again" << std::endl;
		sendto(sock, hand_on_packet, 16, 0, (struct sockaddr*)&send_address, sizeof(send_address));
		std::cerr << "Send on done" << std::endl << std::endl;
		*/
	} 
}

/**
 * Send signal to the Arduino to turn off feedback actions
 */
void hand_off()
{
	if (hand_is_on)
	{
			//PlaySound(NULL, 0, 0);
			//PlaySound(TEXT("VS-end-sound.wav"), NULL, SND_ASYNC); 

		/*
			std::cerr << "Send off" << std::endl;
			sendto(sock, hand_off_packet, 16, 0, (struct sockaddr*)&send_address, sizeof(send_address));
			std::cerr << "Send off again" << std::endl;
			sendto(sock, hand_off_packet, 16, 0, (struct sockaddr*)&send_address, sizeof(send_address));
			std::cerr << "Send off done" << std::endl << std::endl;
		*/
	}
}

void RemoveSound()
{
	waveOutSetVolume(NULL, 10);
	//PlaySound(NULL, 0, 0);
}

void RemoveHaptics()
{
	system("C:/Python33/python.exe gloves.py off");
}