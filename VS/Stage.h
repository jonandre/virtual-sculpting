#pragma once

#ifndef __STAGE_CPP__
extern int stage;
extern int lastWord;
extern int space_pressed;
extern bool hapticsON;
extern bool soundON;
extern bool viewDrawStage;

extern bool reinitiate_acted,	right_acted,		left_acted,				up_acted,
			down_acted,			clockwise_acted,	counter_clockwise_acted,back_acted,
			freez_acted,		clear_acted,		front_acted,			rear_acted,
			top_acted,			bottom_acted,		camera_right_acted,		camera_left_acted, 
			slower_acted,		faster_acted;


extern bool front_view, back_view, top_view, bottom_view, right_view, left_view, freez, clear_view, slower, faster;

extern float actedIndex[24];


extern int acted;
extern bool speechON;

extern bool hand_is_on;
extern bool hapticsConnected;


extern void hand_on();
extern void hand_off();

extern void RemoveSound();
extern void RemoveHaptics();

#endif

int GetStage(void);
void SetStage(int);

bool GetPressedStage(void);
void SetPressedStage(bool);

bool GetHapticsStage(void);
void SetHapticsStage(bool);

bool GetSoundStage(void);
void SetSoundStage(bool);

bool GetReinitiateStage(void);
void SetReinitiateStage(bool);
bool GetRightStage(void);
void SetRightStage(bool);
bool GetLeftStage(void);
void SetLeftStage(bool);
bool GetUpStage(void);
void SetUpStage(bool);
bool GetDownStage(void);
void SetDownStage(bool);
bool GetClockwiseStage(void);
void SetClockwiseStage(bool);
bool GetCounterClockwiseStage(void);
void SetCounterClockwiseStage(bool);
bool GetBackStage(void);
void SetBackStage(bool);
bool GetFreezStage(void);
void SetFreezStage(bool);
bool GetClearStage(void);
void SetClearStage(bool);
bool GetFrontStage(void);
void SetFrontStage(bool);
bool GetRearStage(void);
void SetRearStage(bool);
bool GetTopStage(void);
void SetTopStage(bool);
bool GetBottomStage(void);
void SetBottomStage(bool);
bool GetCameraRightStage(void);
void SetCameraRightStage(bool);
bool GetCameraLeftStage(void);
void SetCameraLeftStage(bool);


bool GetFrontViewStage(void);
void SetFrontViewStage(bool);
bool GetRearViewStage(void);
void SetRearViewStage(bool);
bool GetTopViewStage(void);
void SetTopViewStage(bool);
bool GetBottomViewStage(void);
void SetBottomViewStage(bool);
bool GetRightViewStage(void);
void SetRightViewStage(bool);
bool GetLeftViewStage(void);
void SetLeftViewStage(bool);
bool GetFreezViewStage(void);
void SetFreezViewStage(bool);
bool GetclearViewStage(void);
void SetclearViewStage(bool);


bool GetSlowerStage(void);
void SetSlowerStage(bool);

bool GetFasterStage(void);
void SetFasterStage(bool);

class Stage
{
	public:
		Stage(void);
		~Stage(void);
};

