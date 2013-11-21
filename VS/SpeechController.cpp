#include "SpeechController.h"

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
		/*	Model
		 *	Sculpting
		 *	Begin 
		 */
		case SpeechActionBegin:
			input->space_pressed = true;
			printf("Beginning sculpting. \n");
			break;
			
		/*	Model
		 *	Sculpting 
		 *  End
		 */
		case SpeechActionEnd:
			input->space_pressed = false;
			printf("Ended sculpting. \n");
			break;
			
		/*	Model
		 *	Rotate
		 *  right in +Y
		 */
		case SpeechActionRotateRight:
			input->rotateYGridModel(true);
			printf("Spin right. \n");
			break;
			
		/*	Model
		 *	Rotate
		 *  left in -Y
		 */
		case SpeechActionRotateLeft:
			input->rotateYGridModel(false);
			printf("Spin left. \n");
			break;
			
		/*	Model
		 *	Rotate
		 *  up in +X
		 */
		case SpeechActionRotateUp:
			input->rotateXGridModel(true);
			printf("Spin up. \n");
			break;

		/*	Model
		 *	Rotate
		 *  down in -Z
		 */
		case SpeechActionRotateDown:
			input->rotateXGridModel(false);
			printf("Spin down. \n");
			break;

		/*	Model
		 *	Rotate
		 *  clockwise in +Z
		 */
		case SpeechActionRotateClockwise:
			input->rotateZGridModel(true);
			printf("Spin clockwise. \n");
			break;

		/*	Model
		 *	Rotate
		 *  counter clockwise in -Z
		 */
		case SpeechActionRotateCounterClockwise:
			input->rotateZGridModel(false);
			printf("Spin counter clockwise. \n");
			break;
		
		/*	Model
		 *	New 
		 */
		case SpeechActionNew:
			input->newGridModel(false);
			printf("New model initiated. \n");
			break;
		
		/*	Model
		 *  Cleared
		 */
		case SpeechActionClear:
			input->newGridModel(true);
			printf("Clear model initiated. \n");
			break;
		
		/*	Model
		 *	Rotatrion
		 *  freezed
		 */
		case SpeechActionFreezRotation:
			input->freezGridModel();
			printf("Model rodel freezed. \n");
			break;
		
		/*	Model
		 *	Rotation
		 *  cleared
		 */
		case SpeechActionClearRotation:
			input->freezGridModel();
			input->clearRotationGridModel();
			printf("Model rotation cleared. \n");
			break;

			
		/*	Model
		 *	90 DEGREES 
		 *	right 
		 */
		case SpeechActionRotate90Right:
			input->freezGridModel();
			input->clearRotationGridModel();
			printf("Model rotated 90 degrees right. \n");
			break;

			
		/*	Model
		 *	90 DEGREES 
		 *  left 
		 */
		case SpeechActionRotate90Left:
			input->freezGridModel();
			input->clearRotationGridModel();
			printf("Model rotated 90 degrees left. \n");
			break;
			

		/*	Model
		 *	Rotation
		 *	View
		 *  Top
		 */
		case SpeechActionRotateTopView:
			input->rotateTopView();
			printf("Model rotated to top view. \n");
			break;

		/*	Model
		 *	Rotation
		 *	View
		 *  Left
		 */
		case SpeechActionRotateRightView:
			input->rotateRightView();
			printf("Model rotated to right view. \n");
			break;

		/*	Model
		 *	Rotation
		 *	View
		 *  Right
		 */
		case SpeechActionRotateLeftView:
			input->rotateLeftView();
			printf("Model rotated to left view. \n");
			break;

		/*	Model
		 *	Rotation
		 *	View
		 *  Bottom
		 */
		case SpeechActionRotateBottomView:
			input->rotateBottomView();
			printf("Model rotated to bottom view. \n");
			break;

		/*	Model
		 *	Rotation
		 *	View
		 *  Back
		 */
		case SpeechActionRotateBackView:
			input->rotateBackView();
			printf("Model rotated to back view. \n");
			break;

			
	
		/*	Camera
		 *  Rotation
		 *	View
		 *  Front
		 */
		case SpeechActionCameraRotateFrontView:
			input->cameraRotationViewFront();
			printf("Camera rotated to front view. \n");
			break;
		/*	Camera
		 *  Rotation
		 *	View
		 *  Back
		 */
		case SpeechActionCameraRotateBackView:
			input->cameraRotationViewBack();
			printf("Camera rotated to back view. \n");
			break;
		/*	Camera
		 *  Rotation
		 *	View
		 *  Top
		 */
		case SpeechActionCameraRotateTopView:
			input->cameraRotationViewTop();
			printf("Camera rotated to top view. \n");
			break;
		/*	Camera
		 *  Rotation
		 *	View
		 *  Bottom
		 */
		case SpeechActionCameraRotateBottomView:
			input->cameraRotationViewBottom();
			printf("Camera rotated to bottom view. \n");
			break;
		/*	Camera
		 *  Rotation
		 *	View
		 *  Right
		 */
		case SpeechActionCameraRotateRightView:
			input->cameraRotationViewRight();
			printf("Camera rotated to right view. \n");
			break;
		/*	Camera
		 *  Rotation
		 *	View
		 *  Left
		 */
		case SpeechActionCameraRotateLeftView:
			input->cameraRotationViewLeft();
			printf("Camera rotated to left view. \n");
			break;




		/*	Use
		 *  Haptics
		 */
		case SpeechActionUseHaptics:
			input->UseHaptics();
			printf("Uses of haptics changed");
			break;
		/*	Use 
		 *  Sound
		 */
		case SpeechActionUseSound:
			input->UseSound();
			printf("Usage of sound changed");
			break;



		/*	
		 *	Quit
		 */
		case SpeechActionQuit:
			input->Quit();
			printf("Quit");
			break;
    }
}