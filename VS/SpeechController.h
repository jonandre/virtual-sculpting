#pragma once
#include <stdio.h>
#include "Input.h"

/// <summary>
/// Represents possible actions for Speech, such as moving stoping,
/// rotating to the left, etc.
/// </summary>
enum SpeechAction
{
	////// Stage 1 //////
	SpeechActionModel,
	SpeechActionCamera,
	SpeechActionSound,
	SpeechActionHaptics,

	////// Stage 2 //////
	SpeechActionReinitiate, 
	SpeechActionSpin,
	SpeechActionRotate,
	
	////// Stage 3 || 4 || 5 //////
    SpeechActionRight,
    SpeechActionLeft,

	////// Stage 3 //////
    SpeechActionUp,
    SpeechActionDown,
	SpeechActionClockwise,
	SpeechActionCounterClockwise,
	SpeechActionFreezRotation,
	SpeechActionFaster,
	SpeechActionSlower,
	
	////// Stage 3 || 4 //////
	SpeechActionClearRotation,

	////// Stage 4 || 5 //////
	SpeechActionFront,
	SpeechActionTop,
	SpeechActionBottom,
	SpeechActionRear,

	////// Stage 6 || 7 //////
	SpeechActionON,
	SpeechActionOFF,


	////// Stage 2 || 3 || 4 || 5 || 6 || 7 //////
	SpeechActionBack, 

	////// AL stages //////
    SpeechActionBegin,
    SpeechActionEnd,
	SpeechActionQuit,

	/* Not used right now 
	SpeechActionRotate90Right,
	SpeechActionRotate90Left,
	*/
    SpeechActionNone
};

class SpeechController
{
	public:
		
		void RemoveSound2(void);
		void RemoveHaptics2(void);

		SpeechController(Input* input);
		~SpeechController(void);

		/// <summary>
		/// Makes the program performs the specified action.
		/// </summary>
		void DoAction(SpeechAction action);
		Input* input;
};

