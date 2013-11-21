#pragma once
#include <stdio.h>
#include "Input.h"

/// <summary>
/// Represents possible actions for Speech, such as moving stoping,
/// rotating to the left, etc.
/// </summary>
enum SpeechAction
{
    SpeechActionBegin,
    SpeechActionEnd,

    SpeechActionRotateRight,
    SpeechActionRotateLeft,
    SpeechActionRotateUp,
    SpeechActionRotateDown,
	SpeechActionRotateClockwise,
	SpeechActionRotateCounterClockwise,

	SpeechActionNew,
	SpeechActionClear,

	SpeechActionFreezRotation,
	SpeechActionClearRotation,

	SpeechActionRotate90Right,
	SpeechActionRotate90Left,

	SpeechActionRotateTopView,
	SpeechActionRotateRightView,
	SpeechActionRotateLeftView,
	SpeechActionRotateBottomView,
	SpeechActionRotateBackView,

	
	SpeechActionCameraRotateFrontView,
	SpeechActionCameraRotateBackView,
	SpeechActionCameraRotateTopView,
	SpeechActionCameraRotateBottomView,
	SpeechActionCameraRotateRightView,
	SpeechActionCameraRotateLeftView,

	SpeechActionQuit,

	SpeechActionUseHaptics,
	SpeechActionUseSound,

    SpeechActionNone
};

class SpeechController
{
	public:
		SpeechController(Input* input);
		~SpeechController(void);

		/// <summary>
		/// Makes the program performs the specified action.
		/// </summary>
		void DoAction(SpeechAction action);
		Input* input;
};

