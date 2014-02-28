#include "SpeechController.h"
#include "Stage.h"
#include <Windows.h>




SpeechController::SpeechController(Input* input)
{
	this->input = input;
}
SpeechController::~SpeechController(void)
{
}
/// <summary>
/// Makes the speech commands do performs the specified action.
/// </summary>
void SpeechController::DoAction(SpeechAction action)
{
    switch (action)
    {
		/*	2 || 3 || 4 || 5 || 6 || 7
		 *  Model
		 *	Back
		 */
		case SpeechActionBack:
			if (speechON)
			{
				if (GetStage() == 2 || GetStage() == 3 || 
					GetStage() == 4 || GetStage() == 5 || 
					GetStage() == 6 || GetStage() == 7)
				{
					input->Back();
					SetStage(1);
					printf(" Clearing selections ... \n");
					lastWord = 1;
				}
			}
			break;

			
		/*	Always active
		 *	Sculpting
		 *	Begin 
		 */
		case SpeechActionBegin:
			if (speechON)
			{
				SetPressedStage(true);
				printf("Beginning sculpting. \n");
				lastWord = 2;
			}
			break;
			
		/*	Always active
		 *	Sculpting 
		 *  End
		 */
		case SpeechActionEnd:
			if (speechON)
			{
				SetPressedStage(false);
				printf("Ended sculpting. \n");
				lastWord = 3;
			}
			break;


		/*	1
		 *	 Model
		 */
		case SpeechActionModel:
			if (speechON)
			{
				if (GetStage() == 1 || GetStage() == 3 || GetStage() == 4)
				{
					SetStage(2);
					printf("Model stage initiated... \n");
					lastWord = 4;
				}
			}
			break;
		/*	1
		 *	 Camera
		 */
		case SpeechActionCamera:
			if (speechON)
			{
				if (GetStage() == 1)
				{
					SetStage(5);
					printf("Camera stage initiated... \n");
					lastWord = 20;
				}
			}
			break;
				
		/*	1
		 *	 Sound
		 */
		case SpeechActionSound:
			if (speechON)
			{
				if (GetStage() == 1)
				{
					SetStage(6);
					printf("Sound stage initiated... \n");
					lastWord = 21;
				}
			}
			break;
		/*	1
		 *	Haptics
		 */
		case SpeechActionHaptics:
			if (speechON)
			{
				if (GetStage() == 1)
				{
					SetStage(7);
					printf("Haptics stage initiated... \n");
					lastWord = 24;
				}
			}
			break;
			
		

		/*	2
		 *  Model
		 *	REINITIATE 
		 */
		case SpeechActionReinitiate:
			if (speechON)
			{
				if (GetStage() == 2)
				{
					actedIndex[1] = 0.f;
					input->newGridModel(false);
					printf("Model reinitiated. \n"); 
					SetReinitiateStage(true);
					lastWord = 5;
				}
			}
			break;
				
		/*	3 || 4 || 5
		 *	RIGHT
		 */
		case SpeechActionRight:
			if (speechON)
			{
				if (GetStage() == 3)
				{
					actedIndex[2] = 0.f;
					input->rotateYGridModel(+1);
					printf("Faster RIGHT spinning added to model... \n"); 
					SetRightStage(true);
					lastWord = 19;
				}
				if (GetStage() == 4)
				{
					actedIndex[14] = 0.f;
					input->rotateRightView();
					printf("Model rotated to right view. \n");
					SetRightViewStage(true);
					lastWord = 26;
				}
				if (GetStage() == 5)
				{
					actedIndex[22] = 0.f;
					input->cameraRotationViewRight();
					printf("Camera rotated to right view. \n");
					SetCameraRightStage(true);
					lastWord = 34;
				}
			}
			break;

		/*	3 || 4 || 5
		 *  LEFT
		 */
		case SpeechActionLeft:
			if (speechON)
			{
				if (GetStage() == 3)
				{
					actedIndex[3] = 0.f;
					input->rotateYGridModel(-1);
					printf("Faster LEFT spinning added to model... \n");
					SetLeftStage(true);
					lastWord = 7;
				}
				if (GetStage() == 4)
				{
					actedIndex[15] = 0.f;
					input->rotateLeftView();
					printf("Model rotated to left view. \n");
					SetLeftViewStage(true);
					lastWord = 27;
				}
				if (GetStage() == 5)
				{
					actedIndex[23] = 0.f;
					input->cameraRotationViewLeft();
					printf("Camera rotated to left view. \n");
					SetCameraLeftStage(true);
					lastWord = 35;
				}
			}
			break;



		/*	2
		 *  Model
		 *	Spin
		 */
		case SpeechActionSpin:
			if (speechON)
			{
				if (GetStage() == 2)
				{
					SetStage(3);
					printf(" Spin stage initiated... \n");
					lastWord = 6;
				}
			}
			break;
		/*	2
		 *  Model
		 *	Rotate
		 */
		case SpeechActionRotate:
			if (speechON)
			{
				if (GetStage() == 2)
				{
					SetStage(4);
					printf(" Rotation stage initiated... \n");
					lastWord = 14;
				}
			}
			break;

		/*	3
		 *  Model
		 *	Spin
		 *  up in +X
		 */
		case SpeechActionUp:
			if (speechON)
			{
				if (GetStage() == 3)
				{
					actedIndex[4] = 0.f;
					input->rotateXGridModel(+1);
					printf("Faster UP spinning added to model... \n");
					SetUpStage(true);
					lastWord = 8;
				}
			}
			break;

			
		/*	3
		 *  Model
		 *	Spin
		 *  Faster
		 */
		case SpeechActionFaster:
			if (speechON)
			{
				if (GetStage() == 3)
				{
					actedIndex[24] = 0.f;
					input->rotateFaster();
					printf("Spinning faster... \n");
					SetFasterStage(true);
					lastWord = 36;
				}
			}
			break;

			
		/*	3
		 *  Model
		 *	Spin
		 *  Slower
		 */
		case SpeechActionSlower:
			if (speechON)
			{
				if (GetStage() == 3)
				{
					actedIndex[25] = 0.f;
					input->rotateSlower();
					printf("Spinning slower... \n");
					SetSlowerStage(true);
					lastWord = 37;
				}
			}
			break;

		/*	3
		 *  Model
		 *	Spin
		 *  down in -Z
		 */
		case SpeechActionDown:
			if (speechON)
			{
				if (GetStage() == 3)
				{
					actedIndex[5] = 0.f;
					input->rotateXGridModel(-1);
					printf("Faster DOWN spinning added to model... \n");
					SetDownStage(true);
					lastWord = 9;
				}
			}
			break;

		/*	3
		 *  Model
		 *	Spin
		 *  clockwise in +Z
		 */
		case SpeechActionClockwise:
			if (speechON)
			{
				if (GetStage() == 3)
				{
					actedIndex[6] = 0.f;
					input->rotateZGridModel(+1);
					printf("Faster CLOCkWISE spinning added to model... \n");
					SetClockwiseStage(true);
					lastWord = 10;
				}
			}
			break;
		/*	3
		 *  Model
		 *	Spin
		 *  counter clockwise in -Z
		 */
		case SpeechActionCounterClockwise:
			if (speechON)
			{
				if (GetStage() == 3)
				{
					actedIndex[7] = 0.f;
					input->rotateZGridModel(-1);
					printf("Faster COUNTER CLOCkWISE spinning added to model...  \n");
					SetCounterClockwiseStage(true);
					lastWord = 11;
				}
			}
			break;
		/*	3
		 *  Model
		 *	Spinning
		 *  freezed
		 */
		case SpeechActionFreezRotation:
			if (speechON)
			{
				if (GetStage() == 3 || GetStage() == 4)
				{
					input->freezGridModel();
					printf("Model spinning FREEZED.  \n");
					lastWord = 12;
				}
				if (GetStage() == 3)
				{
					actedIndex[8] = 0.f;
					SetFreezStage(true);
					lastWord = 12;
				}
				if (GetStage() == 4)
				{
					actedIndex[16] = 0.f;
					SetFreezViewStage(true);
					lastWord = 28;
				}
			}
			break;

			

		/*	3 || 4
		 *  Model
		 *	Rotation
		 *  cleared
		 */

		case SpeechActionClearRotation:
			if (speechON)
			{
				if (GetStage() == 3 || GetStage() == 4)
				{
					input->freezGridModel();
					input->clearRotationGridModel();
					printf("Model rotation CLEARED. \n");
					lastWord = 13;
				}
				if (GetStage() == 3)
				{
					actedIndex[9] = 0.f;
					SetClearStage(true);
					lastWord = 13;
				}
				if (GetStage() == 4)
				{
					actedIndex[17] = 0.f;
					SetclearViewStage(true);
					lastWord = 29;
				}
			}
			break;

		/*	4 || 5
		 *  FRONT
		 */
		case SpeechActionFront:
			if (speechON)
			{
				if (GetStage() == 4)
				{
					actedIndex[10] = 0.f;
					input->freezGridModel();
					input->clearRotationGridModel();
					printf("Model rotated to front view. \n");
					SetFrontViewStage(true);
					lastWord = 15;
				}
				if (GetStage() == 5)
				{
					actedIndex[18] = 0.f;
					input->cameraRotationViewFront();
					printf("Camera rotated to front view. \n");
					SetFrontStage(true);
					lastWord = 30;
				}
			}
			break;
			
		/*	4 || 5
		 *  TOP
		 */
		case SpeechActionTop:
			if (speechON)
			{
				if (GetStage() == 4)
				{
					actedIndex[12] = 0.f;
					input->rotateTopView();
					printf("Model rotated to top view. \n");
					SetTopViewStage(true);
					lastWord = 17;
				}
				if (GetStage() == 5)
				{
					actedIndex[20] = 0.f;
					input->cameraRotationViewTop();
					printf("Camera rotated to top view. \n");
					SetTopStage(true);
					lastWord = 32;
				}
			}
			break;
			
			
		
			
		/*	4 || 5
		 *  BOTTOM
		 */
		case SpeechActionBottom:
			if (speechON)
			{
				if (GetStage() == 4)
				{
					actedIndex[13] = 0.f;
					input->rotateBottomView();
					printf("Model rotated to bottom view. \n");
					SetBottomViewStage(true);
					lastWord = 18;
				}
				if (GetStage() == 5)
				{
					actedIndex[21] = 0.f;
					input->cameraRotationViewBottom();
					printf("Camera rotated to bottom view. \n");
					SetBottomStage(true);
					lastWord = 33;
				}
			}
			break;

		/*	4 || 5
		 *  Model
		 *	Rotation
		 *  Back
		 *	View
		 */
		case SpeechActionRear:
			if (speechON)
			{
				if (GetStage() == 4)
				{
					actedIndex[11] = 0.f;
					input->rotateBackView();
					printf("Model rotated to back view. \n");
					SetRearViewStage(true);
					lastWord = 16;
				}
				if (GetStage() == 5)
				{
					actedIndex[19] = 0.f;
					input->cameraRotationViewBack();
					printf("Camera rotated to back view. \n");
					SetRearStage(true);
					lastWord = 31;
				}
			}
			break;
			
			
		/*	6 || 7
		 *	ON
		 */
		case SpeechActionON:
			if (speechON)
			{
				if (GetStage() == 6)
				{
					SetSoundStage(true);
					printf("Sounds is sett to ON. \n");
					lastWord = 22;
				}
				if (GetStage() == 7)
				{
					SetHapticsStage(true);
					printf("Haptics is sett to ON. \n");
					lastWord = 22;
				}
			}
			break;

		/*	6 || 7
		 *	OFF
		 */
		case SpeechActionOFF:
			if (speechON)
			{
				if (GetStage() == 6)
				{
					SetSoundStage(false);
					printf("Sounds is sett to OFF. \n");
					lastWord = 23;
				}
				if (GetStage() == 7)
				{
					SetHapticsStage(false);
					printf("Haptics is sett to OFF. \n");
					lastWord = 23;
				}
			}
			break;


			

		/*	Always active
		 *	Quit
		 */
		case SpeechActionQuit:
			if (speechON)
			{
				input->Quit();
				printf("Quit \n");
				lastWord = 25;
			}
			break;


		/*	Model
		 *	90 DEGREES 
		 *	right 
		 *//*
		case SpeechActionRotate90Right:
			input->freezGridModel();
			input->clearRotationGridModel();
			printf("Model rotated 90 degrees right. \n");
			break;
		/*	Model
		 *	90 DEGREES 
		 *  left 
		 *//*
		case SpeechActionRotate90Left:
			input->freezGridModel();
			input->clearRotationGridModel();
			printf("Model rotated 90 degrees left. \n");
			break;
		*/
    }
}
