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
    float rad_map = glm::pi<float>()/180.0;

    switch (action)
    {
    case SpeechActionStart:
        input->space_pressed = true;
		printf("start \n");
		break;

    case SpeechActionStop:
        input->space_pressed = false;
		printf("stop \n");
		break;

    case SpeechActionRotateRight:
        input->_rotation_vector_obj.y += 0.2f*rad_map;
		printf("rotate right \n");
        break;

    case SpeechActionRotateLeft:
		input->_rotation_vector_obj.y -= 0.2f*rad_map;
		printf("rotate left \n");
        break;
    }

}