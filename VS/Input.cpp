#include "Input.h"
#include "GridModel.h"

Input::Input():_lbtn_pressed(false), zoom_val(0.0)
{
	_angleXS = 0;
	_angleYS = 0;
	//_obj_mat = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f));
	_obj_quat = glm::quat( glm::vec3(0.0));
	_view_mat = glm::mat4(1.0f);
	_model = NULL;
	_rotation_vector_obj = glm::vec3(0.0,0.0,0.0);
	space_pressed = false;
}


Input::~Input(void)
{
} 
void Input::newGridModel(bool b)
{
	_model->ReInitModel( b );
}

float rad_map = glm::pi<float>()/180.0;
void Input::rotateYGridModel(bool directionRight)
{
	if (directionRight)
		_rotation_vector_obj.y += 0.2f*rad_map;
	else
		_rotation_vector_obj.y -= 0.2f*rad_map;
}
void Input::rotateXGridModel(bool directionUp)
{
	if (directionUp)
		_rotation_vector_obj.x -= 0.2f*rad_map;
	else
		_rotation_vector_obj.x += 0.2f*rad_map;
}
void Input::rotateZGridModel(bool directionClockwise)
{
	if (directionClockwise)
		_rotation_vector_obj.z -= 0.2f*rad_map;
	else
		_rotation_vector_obj.z += 0.2f*rad_map;
}
void Input::freezGridModel()
{
	_rotation_vector_obj = glm::vec3(0.0,0.0,0.0);
}
void Input::clearRotationGridModel()
{
	_obj_quat = glm::quat();
}


void Input::rotate90DegreesRightY()
{
	_rotation_vector_obj = glm::vec3(0.0,glm::pi<float>()/2,0.0);
	_obj_quat = glm::normalize(_obj_quat * glm::quat(_rotation_vector_obj));
	_rotation_vector_obj = glm::vec3(0.0,0.0,0.0);
}
void Input::rotate90DegreesLeftY()
{
	_rotation_vector_obj = glm::vec3(0.0,glm::pi<float>()/2,0.0);
	_obj_quat = glm::normalize(_obj_quat * glm::quat(_rotation_vector_obj));
	_rotation_vector_obj = glm::vec3(0.0,0.0,0.0);
}



void Input::rotateTopView()
{
	_obj_quat = glm::quat();

	_rotation_vector_obj = glm::vec3(glm::pi<float>()/2,0.0,0.0);
	_obj_quat = glm::normalize(_obj_quat * glm::quat(_rotation_vector_obj));
	_rotation_vector_obj = glm::vec3(0.0,0.0,0.0);
}
void Input::rotateRightView()
{
	_obj_quat = glm::quat();

	_rotation_vector_obj = glm::vec3(0.0,glm::pi<float>()/2,0.0);
	_obj_quat = glm::normalize(_obj_quat * glm::quat(_rotation_vector_obj));
	_rotation_vector_obj = glm::vec3(0.0,0.0,0.0);
}
void Input::rotateLeftView()
{
	_obj_quat = glm::quat();

	_rotation_vector_obj = glm::vec3(0.0,-glm::pi<float>()/2,0.0);
	_obj_quat = glm::normalize(_obj_quat * glm::quat(_rotation_vector_obj));
	_rotation_vector_obj = glm::vec3(0.0,0.0,0.0);
}
void Input::rotateBottomView()
{
	_obj_quat = glm::quat();
	
	_rotation_vector_obj = glm::vec3(-glm::pi<float>()/2,0.0,0.0);
	_obj_quat = glm::normalize(_obj_quat * glm::quat(_rotation_vector_obj));
	_rotation_vector_obj = glm::vec3(0.0,0.0,0.0);
}
void Input::rotateBackView()
{
	_obj_quat = glm::quat();

	_rotation_vector_obj = glm::vec3(0.0,glm::pi<float>(),0.0);
	_obj_quat = glm::normalize(_obj_quat * glm::quat(_rotation_vector_obj));
	_rotation_vector_obj = glm::vec3(0.0,0.0,0.0);
}

/** Camera **/
/* Camera
 * Rotation
 * View 
 * Front
 */
void Input::cameraRotationViewFront()
{
	_view_mat = glm::mat4(1.0);
}
/* Camera
 * Rotation
 * View 
 * Back
 */
void Input::cameraRotationViewBack()
{
	_view_mat = glm::rotate( glm::mat4(1.0), 180.0f, glm::vec3(0.0, 1.0, 0.0) );
}
/* Camera
 * Rotation
 * View 
 * Top
 */
void Input::cameraRotationViewTop()
{
	_view_mat = glm::rotate( glm::mat4(1.0), 90.0f, glm::vec3(1.0, 0.0, 0.0) );
}
/* Camera
 * Rotation
 * View 
 * Bottom
 */
void Input::cameraRotationViewBottom()
{
	_view_mat = glm::rotate( glm::mat4(1.0), -90.0f, glm::vec3( 1.0, 0.0, 0.0) );
}
/* Camera
 * Rotation
 * View 
 * Left
 */
void Input::cameraRotationViewLeft()
{
	_view_mat = glm::rotate( glm::mat4(1.0), 90.0f, glm::vec3(0.0, 1.0, 0.0) );
}
/* Camera
 * Rotation
 * View 
 * Right
 */
void Input::cameraRotationViewRight()
{
	_view_mat = glm::rotate( glm::mat4(1.0), -90.0f, glm::vec3(0.0, 1.0, 0.0) );
}

/** Use haptics on or off **/
void Input::UseHaptics()
{
	if (useHaptics == true)
		useHaptics = false;
	else
		useHaptics = true;
}

/** Use sound on or off **/
void Input::UseSound()
{
	if (useSound == true)
		useSound = false;
	else
		useSound = true;
}

/** Quit program **/
void Input::Quit()
{
	_exit(0);
}



void Input::OnKeyPressed( char c )
{
	if ( c == 'R' )
	{
		freezGridModel();
		clearRotationGridModel();
	}
	else if (c == 'F')
		freezGridModel();
	else if (c == 'I')
		newGridModel(false);
	else if (c == 'C')
		newGridModel(true);

	else if (c == 'D')
		rotateYGridModel(true);
	else if (c == 'A')
		rotateYGridModel(false);

	else if (c == 'W')
		rotateXGridModel(true);
	else if (c == 'S')
		rotateXGridModel(false);

	else if (c == 'Q')
		rotateZGridModel(true);
	else if (c == 'E')
		rotateZGridModel(false);

	else if (c == 'K')
		UseHaptics();
	else if (c == 'L')
		UseSound();

	else if (c == '4')
		cameraRotationViewLeft();
	else if (c == '6')
		cameraRotationViewRight();
	else if (c == '2')
		cameraRotationViewBottom();
	else if (c == '8')
		cameraRotationViewTop();
	else if (c == '5')
		cameraRotationViewFront();
	else
		_pressed_keys.push_back(c);
	
}


bool Input::IsPressed( char c )
{
	for (int i = 0; i < _pressed_keys.size(); i++)
	{
		if ( _pressed_keys[i] == c )
			return true;
	}

	return false;
}

void Input::OnMouseLBDown( int x, int y )
{
	_mouseStartX = x;
	_mouseStartY = y;
	_prevX = x;
	_prevY = y;
	_lbtn_pressed = true;
}

glm::mat4 Input::GetObjectM()
{
	return glm::toMat4(_obj_quat);
}

glm::quat Input::GetObjectQ()
{
	return glm::normalize(_obj_quat);
}

void Input::UpdateFrame()
{
	{
		_obj_quat = glm::normalize(_obj_quat * glm::quat(_rotation_vector_obj));
	}
	_pressed_keys.clear();
}



glm::mat4 Input::GetViewM()
{
	return glm::translate(glm::mat4(1.0), glm::vec3(0.f, 0.f, zoom_val))*_view_mat;
}

void Input::SetViewM(const glm::mat4& mat )
{
	_view_mat = mat;
}

void Input::SetZoom( float val )
{
	zoom_val = val;
}

void Input::SetModel( GridModel* md )
{
	_model = md;
}

void Input::OnMouseMove( int x, int y )
{
	if (_lbtn_pressed)
	{
		float rotSpeed = 0.1f;
		_angleX =  (x - _prevX) * rotSpeed;
		_angleY =  (y - _prevY) * rotSpeed;
		glm::vec3 axis(_angleY, _angleX, 0);
		
		float angle = glm::length(axis);
		if ( abs(angle) > 0.1 )
		{
			axis = glm::normalize(axis);			
			_view_mat = glm::rotate(glm::mat4(1.0), angle, axis)*_view_mat;
		}
		_prevX = x;
		_prevY = y;
	}
}

void Input::OnSroll( int dx )
{
	float step = 8.0f*float(dx)/120;
	zoom_val += step;
}

void Input::OnMouseLBUp( int x, int y )
{
	_lbtn_pressed = false;
	_angleXS = _angleX;
    _angleYS = _angleY;
}

