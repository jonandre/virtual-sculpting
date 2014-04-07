#include "Input.h"
#include "Stage.h"


static const float DEGREES_PER_SECOND_PER_SECOND = glm::pi<float>() * 30.f / 1000.f / 180.f;
#define DEGREES_PER_SECOND DEGREES_PER_SECOND_PER_SECOND

static glm::vec3 rotation = glm::vec3(1.f, 0.f, 0.f);
static float rotation_speed = 0.f;
static bool rotation_freeze = false;

float fps_procentage = 1.f;
bool fps_regulation = true;

glm::mat4 transformation = glm::mat4(1.f);


Input::Input():_lbtn_pressed(false), zoom_val(0.0)
{
	_angleXS = 0;
	_angleYS = 0;
	_obj_quat = glm::quat( glm::vec3(0.0));
	_view_mat = glm::mat4(1.0f);
	_model = NULL;
	_rotation_vector_obj = glm::vec3(0.0,0.0,0.0);
	this->rx = 0;
	this->ry = 0;
	this->rz = 0;
	
	std::cout << "Input Initialized" << std::endl;
}

Input::~Input(void)
{
} 

void Input::Back(void)
{
	actedIndex[0] = 0;
	SetBackStage(true);
	
}
void Input::newGridModel(bool b)
{
	_model->ReInitModel( b );
}



void Input::freezGridModel()
{
	rotation_freeze = true;
}
void Input::clearRotationGridModel()
{
	_obj_quat = glm::quat(glm::mat4(1.f));
}

void Input::rotateYGridModel(int directionRight)
{
	ry = directionRight;
}
void Input::rotateXGridModel(int directionUp)
{
	rx = directionUp;
}
void Input::rotateZGridModel(int directionClockwise)
{
	rz = directionClockwise;
}
void Input::rotateFaster(void)
{
	rotation_speed *= 2.0f;
}
void Input::rotateSlower(void)
{
	rotation_speed /= 2.0f;
}

void Input::rotateTopView()
{
	_obj_quat = glm::quat();

	_rotation_vector_obj = glm::vec3(glm::pi<float>()/2,0.0,0.0);
	_obj_quat = glm::normalize(_obj_quat * glm::quat(_rotation_vector_obj));
	_rotation_vector_obj = glm::vec3(0.0,0.0,0.0);
	freezGridModel();
}
void Input::rotateRightView()
{
	_obj_quat = glm::quat();

	_rotation_vector_obj = glm::vec3(0.0,glm::pi<float>()/2,0.0);
	_obj_quat = glm::normalize(_obj_quat * glm::quat(_rotation_vector_obj));
	freezGridModel();
}
void Input::rotateLeftView()
{
	_obj_quat = glm::quat();

	_rotation_vector_obj = glm::vec3(0.0,-glm::pi<float>()/2,0.0);
	_obj_quat = glm::normalize(_obj_quat * glm::quat(_rotation_vector_obj));
	freezGridModel();
}
void Input::rotateBottomView()
{
	_obj_quat = glm::quat();
	
	_rotation_vector_obj = glm::vec3(-glm::pi<float>()/2,0.0,0.0);
	_obj_quat = glm::normalize(_obj_quat * glm::quat(_rotation_vector_obj));
	freezGridModel();
}
void Input::rotateBackView()
{
	_obj_quat = glm::quat();

	_rotation_vector_obj = glm::vec3(0.0,glm::pi<float>(),0.0);
	_obj_quat = glm::normalize(_obj_quat * glm::quat(_rotation_vector_obj));
	freezGridModel();
}

void Input::cameraRotationViewFront()
{
	_view_mat = glm::mat4(1.0);
}
void Input::cameraRotationViewBack()
{
	_view_mat = glm::rotate( glm::mat4(1.0), 180.0f, glm::vec3(0.0, 1.0, 0.0) );
}
void Input::cameraRotationViewTop()
{
	_view_mat = glm::rotate( glm::mat4(1.0), 90.0f, glm::vec3(1.0, 0.0, 0.0) );
}
void Input::cameraRotationViewBottom()
{
	_view_mat = glm::rotate( glm::mat4(1.0), -90.0f, glm::vec3( 1.0, 0.0, 0.0) );
}
void Input::cameraRotationViewLeft()
{
	_view_mat = glm::rotate( glm::mat4(1.0), 90.0f, glm::vec3(0.0, 1.0, 0.0) );
}
void Input::cameraRotationViewRight()
{
	_view_mat = glm::rotate( glm::mat4(1.0), -90.0f, glm::vec3(0.0, 1.0, 0.0) );
}

/** Quit program **/
void Input::Quit()
{
	RemoveSound();

	if (hapticsConnected)
		RemoveHaptics();
	_exit(0);
}



void Input::OnKeyPressed( SDL_Keycode c )
{
	switch (c)
	{
		// Clear rotation...
		case SDLK_r:
			clearRotationGridModel();
		case SDLK_f:
			freezGridModel();	//Freeze
			break;

		case SDLK_i:   newGridModel(false);		break;
		case SDLK_c:   newGridModel(true);			break;
			
		/// X Vertical rotation
		case SDLK_w:  rotateXGridModel(+1);		break;
		case SDLK_s:  rotateXGridModel(-1);		break;

		/// Y Horizontal rotation
		case SDLK_d:  rotateYGridModel(+1);		break;
		case SDLK_a:  rotateYGridModel(-1);		break;

		/// Z Clockwise rotation
		case SDLK_q:  rotateZGridModel(-1);		break;
		case SDLK_e:  rotateZGridModel(+1);		break;
			
		// Rotation increase/decrease
		case SDLK_g:  rotateFaster();				break;
		case SDLK_v:  rotateSlower();				break;

		/// Haptics
		case SDLK_h:  SetHapticsStage(true);		break;
		case SDLK_j:  SetHapticsStage(false);		break;

		/// Haptics
		case SDLK_k:  SetSoundStage(true);			break;
		case SDLK_l:  SetSoundStage(false);		break;

		/// Camera
		case SDLK_4:  cameraRotationViewLeft();    break;
		case SDLK_6:  cameraRotationViewRight();   break;
		case SDLK_2:  cameraRotationViewBottom();  break;
		case SDLK_8:  cameraRotationViewTop();		break;
		case SDLK_5:  cameraRotationViewFront();	break;

		case SDLK_TAB:  viewDrawStage ^= true;		break;

		/// ESC
		case SDLK_ESCAPE:  Quit();					break;
		case SDLK_p: speechON ^= true;				break;
		case SDLK_o: hapticsConnected ^= true;			break;
			
		/// ON OFF sculpting
		case SDLK_SPACE:
			SetPressedStage(GetPressedStage() ^ true);
			break;
		case SDLK_x:
			fps_regulation ^= true;
			break;
		
	}
}
void Input::OnMouseLBDown( int x, int y )
{
	_mouseStartX = x;
	_mouseStartY = y;
	_prevX = x;
	_prevY = y;
	_lbtn_pressed = true;
}

/** 
 * Get projection matrix
 */
glm::mat4 Input::GetObjectM()
{
	//return transformation;
	return glm::toMat4(_obj_quat);
}

/** 
 * Get projection quaterion
 */
glm::quat Input::GetObjectQ()
{
	//return glm::normalize(_obj_quat);
	//return glm::quat(transformation);
	return _obj_quat;
}

glm::mat4 createRotationMatrix(glm::vec3, float);

/** 
 * Get projection quaterion
 */
void Input::UpdateFrame()
{
	float ACCELERATION = 10.0f * DEGREES_PER_SECOND_PER_SECOND;
	
	if (rotation_freeze)
	{
		rotation_speed = 0;
		rotation_freeze = false;
	}
	
	if (rx != 0)
	{
		glm::vec3 v = this->rx * ACCELERATION * glm::vec3(1.f, 0.f, 0.f);
		v += rotation_speed * rotation;
		rotation_speed = glm::length(v);
		if (rotation_speed * rotation_speed > 0.000001f)
			rotation = glm::normalize(v);
		else
		{
			rotation = glm::vec3(1, 0, 0);
			rotation_speed = 0;
		}
	}
	if (ry != 0)
	{
		glm::vec3 v = this->ry * ACCELERATION * glm::vec3(0.f, -1.f, 0.f);
		v += rotation_speed * rotation;
		rotation_speed = glm::length(v);
		if (rotation_speed * rotation_speed > 0.000001f)
			rotation = glm::normalize(v);
		else
		{
			rotation = glm::vec3(1, 0, 0);
			rotation_speed = 0;
		}
	}
	if (rz != 0)
	{
		glm::vec3 v = this->rz * ACCELERATION * glm::vec3(0.f, 0.f, 1.f);
		v += rotation_speed * rotation;
		rotation_speed = glm::length(v);
		if (rotation_speed * rotation_speed > 0.000001f)
			rotation = glm::normalize(v);
		else
		{
			rotation = glm::vec3(1, 0, 0);
			rotation_speed = 0;
		}
	}
	rx = ry = rz = 0;

	if (rotation_speed > 360.f * DEGREES_PER_SECOND)
		rotation_speed = 360.f * DEGREES_PER_SECOND;

	if (fps_regulation)
		transformation = createRotationMatrix(rotation, rotation_speed * fps_procentage);
	else
		transformation = createRotationMatrix(rotation, rotation_speed);

	_obj_quat = glm::quat(transformation * glm::toMat4(_obj_quat));
}


inline glm::mat4 createRotationMatrix(glm::vec3 direction, float theta)
{
	glm::mat4 m = glm::mat4();

	float x = direction[0];
	float y = direction[1];
	float z = direction[2];

	float cos = glm::cos(theta);
	float sin = glm::sin(theta);
	float _cos = 1.f - cos;

	m[0][0] = x * x * _cos + cos;
	m[1][0] = x * y * _cos + z * sin;
	m[2][0] = x * z * _cos - y * sin;

	m[0][1] = y * x * _cos - z * sin;
	m[1][1] = y * y * _cos + cos;
	m[2][1] = y * z * _cos + x * sin;

	m[0][2] = z * x * _cos + y * sin;
	m[1][2] = z * y * _cos - x * sin;
	m[2][2] = z * z * _cos + cos;
	
	m[0][3] = m[1][3] = m[2][3] = 0;
	m[3][0] = m[3][1] = m[3][2] = 0;
	m[3][3] = 1;

	return m;
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

float Input::GetZoom ()
{
	return zoom_val;
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

