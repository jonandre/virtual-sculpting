#pragma once
#include <stdio.h>
#include "Input.h"
/* Speech start */
/// <summary>
/// Represents possible actions for Speech, such as moving stoping,
/// rotating to the left, etc.
/// </summary>
enum SpeechAction
{
    SpeechActionStart,
    SpeechActionStop,
    SpeechActionRotateRight,
    SpeechActionRotateLeft,
    SpeechActionNone
};
/* Speech end */

class SpeechController
{
public:
	SpeechController(Input* input);
	~SpeechController(void);
	/* Speech start */

	/// <summary>
    /// Makes the program performs the specified action.
    /// </summary>
	/* Speech end */
    void DoAction(SpeechAction action);
	Input* input;
};

