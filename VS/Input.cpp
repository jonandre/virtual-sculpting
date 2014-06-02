#include "Input.h"
#include "Stage.h"


static const float DEGREES_PER_SECOND_PER_SECOND = glm::pi<float>() * 30.f / 1000.f / 180.f;
#define DEGREES_PER_SECOND DEGREES_PER_SECOND_PER_SECOND

static glm::vec3 rotation = glm::vec3(1.f, 0.f, 0.f);
static float rotation_speed = 0.f;
static bool rotation_freeze = false;
static float rotation_speed_increase = 5.0f;

float fps_procentage = 1.f;
bool fps_regulation = true;

glm::mat4 transformation = glm::mat4(1.f);


Input::Input():_lbtn_pressed(false), zoom_val(0.0)
{
	_angleXS = 0;
	_angleYS = 0;
	_obj_quat = glm::quat(glm::vec3(0.0));
	rotSpeed = glm::vec3(0.0f);
	wantedPos = glm::vec3(0.0f);
	_obj_pos = glm::vec3(0.0f);
	_obj_scale = 1.0;
	wantedScale = 1.0f;
	_view_mat = glm::mat4(1.0f);
	_model = NULL;
	_rotation_vector_obj = glm::vec3(0.0,0.0,0.0);
	this->rx = 0;
	this->ry = 0;
	this->rz = 0;
	handPosition = glm::vec3(0.0f);
	handVelocity = glm::vec3(0.0f);


	rotateOn = false;
	
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
	rotSpeed = glm::vec3(0.0f);
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

void Input::TranslateGridModel(glm::vec3& translation)
{
	wantedPos += translation;
}

glm::vec3 Input::GetRotationFromTo(glm::vec3& from, glm::vec3& to)
{
	glm::vec3 f = from - _obj_pos;
	glm::vec3 t = to - _obj_pos;

	glm::vec3 rot;
	rot.x = -glm::degrees( glm::atan(t.y/t.z) - glm::atan(f.y/f.z));
	rot.y = glm::degrees( glm::atan(t.x/t.z) - glm::atan(f.x/f.z));

	return rot;
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
		case SDLK_w:  rotSpeed.x -= rotation_speed_increase;		break;
		case SDLK_s:  rotSpeed.x += rotation_speed_increase;		break;

		/// Y Horizontal rotation
		case SDLK_d:  rotSpeed.y += rotation_speed_increase;		break;
		case SDLK_a:  rotSpeed.y -= rotation_speed_increase;		break;

		/// Z Clockwise rotation
		case SDLK_q:  rotSpeed.z += rotation_speed_increase;		break;
		case SDLK_e:  rotSpeed.z -= rotation_speed_increase;		break;
			
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
		case SDLK_RETURN:
			SetPressedStage(GetPressedStage() ^ true);
			break;
		case SDLK_SPACE:
			rotSpeed = glm::vec3(0.0f);
			lastHandPosition = handPosition;

			rotateOn = true;
			break;
		case SDLK_x:
			//fps_regulation ^= true;
			STLExporter::ExportToStl(_model->GetCells(), _model->GetDimm());

			break;
		
		case SDLK_RIGHT:
			wantedPos += glm::vec3(0.0f, 0.0f, 0.5f);
			break;
		case SDLK_LEFT:
			wantedPos -= glm::vec3(0.0f, 0.0f, 0.5f);
			break;
		case SDLK_PAGEDOWN:
		case SDLK_PAGEUP:
			rotSpeed = glm::vec3(0.0f);
			lastHandPosition = handPosition;

			rotateOn = true;
			break;
		case SDLK_UP:
			wantedPos += glm::vec3(0.0f, 0.25f, 0.0f);
			break;
		case SDLK_DOWN:
			wantedPos -= glm::vec3(0.0f, 0.25f, 0.0f);
			break;
		case SDLK_PLUS:
			wantedScale *=  1.2f;
			break;
		case SDLK_MINUS:
			wantedScale /= 1.2f;
			break;
	}
}

void Input::OnKeyReleased( SDL_Keycode c )
{
	switch (c)
	{
		case SDLK_SPACE:
		case SDLK_PAGEDOWN:
		case SDLK_PAGEUP:
			if (glm::length(handVelocity) > 0.05f) // m/s
				rotSpeed = GetRotationFromTo(lastHandPosition, lastHandPosition + handVelocity);
			else
				rotSpeed = glm::vec3(0.0f);

			rotateOn = false;
			break;
		default:
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
	glm::mat4 rot = glm::toMat4(_obj_quat);

	glm::mat4 m = glm::translate(glm::mat4(1.0f), _obj_pos);
	m = glm::scale(m, glm::vec3(_obj_scale));
	m = m*rot;
	return m;
}

glm::mat4 Input::GetModelM()
{
	glm::mat4 m = glm::translate(glm::mat4(1.0f), _obj_pos);
	m = glm::scale(m, glm::vec3(_obj_scale));
	return m;
}

/** 
 * Get projection quaterion
 */
glm::quat Input::GetObjectQ()
{
	return _obj_quat;
}

glm::mat4 createRotationMatrix(glm::vec3, float);

void Input::UpdateFrame(float deltaTime)
{	
	glm::quat aux (glm::vec3(0.0f));
	if (rotateOn) {
		glm::vec3 vel = (handPosition - lastHandPosition)/deltaTime;
		handVelocity = 0.9f*handVelocity + 0.1f*vel;

		glm::vec3 r = GetRotationFromTo(lastHandPosition, handPosition);
		aux = glm::rotate(aux, r.x, glm::vec3(1.0f,0.0f,0.0f));
		aux = glm::rotate(aux, r.y, glm::vec3(0.0f,1.0f,0.0f));
		aux = glm::rotate(aux, r.z, glm::vec3(0.0f,0.0f,1.0f));

		lastHandPosition = handPosition;
	}
	else {
		aux = glm::rotate(aux, rotSpeed.x*deltaTime, glm::vec3(1.0f,0.0f,0.0f));
		aux = glm::rotate(aux, rotSpeed.y*deltaTime, glm::vec3(0.0f,1.0f,0.0f));
		aux = glm::rotate(aux, rotSpeed.z*deltaTime, glm::vec3(0.0f,0.0f,1.0f));
	}

	_obj_quat = aux*_obj_quat;

	float speed = 1.0f;
	_obj_pos = _obj_pos*(speed - deltaTime)/speed + wantedPos*deltaTime/speed;
	_obj_scale = _obj_scale*(speed - deltaTime)/speed + wantedScale*deltaTime/speed;
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

void Input::SetModelPosition(glm::vec3 pos)
{
	wantedPos = pos;
	_obj_pos = pos;
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

void Input::SetModelSide( float wantedSide)
{
	this->wantedSide = wantedSide;
	wantedScale = wantedSide/float(_model->GetDimm());
	_obj_scale = wantedScale;
}

float Input::GetModelSide()
{
	return wantedSide;
}

glm::vec3 Input::GetObjectPosition()
{
	return _obj_pos;
}

void Input::UpdateHandPosition(glm::vec3& lHand, glm::vec3& rHand)
{
	handPosition = lHand.y > rHand.y? lHand : rHand;
}