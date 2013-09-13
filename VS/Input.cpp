#include "Input.h"
#include "GridModel.h"


Input::Input() : _lbtn_pressed(false), zoom_val(0.0)
{
	_angleXS = 0;
	_angleYS = 0;
	//_obj_mat = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f));
	_obj_quat = glm::quat(glm::vec3(0.0));
	_view_mat = glm::mat4(1.0f);
	_model = NULL;
	_rotation_vector_obj = glm::vec3(0.0,0.0,0.0);
}


Input::~Input(void)
{
} 


void Input::OnKeyPressed(char c)
{
	float rad_map = glm::pi<float>() / 180.0;
	if (c == 'R')
	{
		_rotation_vector_obj = glm::vec3(0.0,0.0,0.0);
		_obj_quat = glm::quat();
	}
	else if (c == 'F')  _rotation_vector_obj = glm::vec3(0.0,0.0,0.0);
	else if (c == 'I')  _model->ReInitModel(false);
	else if (c == 'C')  _model->ReInitModel(true ;
	else if (c == 'D')  _rotation_vector_obj.y += 0.2f * rad_map;
	else if (c == 'A')  _rotation_vector_obj.y -= 0.2f * rad_map;
	else if (c == 'W')  _rotation_vector_obj.x += 0.2f * rad_map;
	else if (c == 'S')  _rotation_vector_obj.x -= 0.2f * rad_map;
	else if (c == 'Q')  _rotation_vector_obj.z += 0.2f * rad_map;
	else if (c == 'E')  _rotation_vector_obj.z -= 0.2f * rad_map;
	else if (c == '4')  _view_mat = glm::rotate(glm::mat4(1.0), -90.0f, glm::vec3(0.0, 1.0, 0.0));
	else if (c == '6')  _view_mat = glm::rotate(glm::mat4(1.0),  90.0f, glm::vec3(0.0, 1.0, 0.0));
	else if (c == '2')  _view_mat = glm::rotate(glm::mat4(1.0), -90.0f, glm::vec3(1.0, 0.0, 0.0));
	else if (c == '8')  _view_mat = glm::rotate(glm::mat4(1.0),  90.0f, glm::vec3(1.0, 0.0, 0.0));
	else if (c == '5')  _view_mat = glm::mat4(1.0);
	else                _pressed_keys.push_back(c);
}


bool Input::IsPressed(char c)
{
	for (int i = 0; i < _pressed_keys.size(); i++)
		if (_pressed_keys[i] == c)
			return true;
	
	return false;
}

void Input::OnMouseLBDown(int x, int y)
{
	_prevX = _mouseStartX = x;
	_prevY = _mouseStartY = y;
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
	_obj_quat = glm::normalize(_obj_quat * glm::quat(_rotation_vector_obj));
	_pressed_keys.clear();
}



glm::mat4 Input::GetViewM()
{
	return glm::translate(glm::mat4(1.0), glm::vec3(0.f, 0.f, zoom_val)) * _view_mat;
}

void Input::SetViewM(const glm::mat4& mat)
{
	_view_mat = mat;
}

void Input::SetZoom(float val)
{
	zoom_val = val;
}

void Input::SetModel(GridModel* md)
{
	_model = md;
}

void Input::OnMouseMove(int x, int y)
{
	if (_lbtn_pressed)
	{
		float rotSpeed = 0.1f;
		_angleX = (x - _prevX) * rotSpeed;
		_angleY = (y - _prevY) * rotSpeed;
		glm::vec3 axis(_angleY, _angleX, 0);
		
		float angle = glm::length(axis);
		if ( abs(angle) > 0.1)
		{
			axis = glm::normalize(axis);			
			_view_mat = glm::rotate(glm::mat4(1.0), angle, axis)*_view_mat;
		}
		_prevX = x;
		_prevY = y;
	}
}

void Input::OnSroll(int dx)
{
	zoom_val += dx / 15.0f;
}

void Input::OnMouseLBUp(int x, int y)
{
	_lbtn_pressed = false;
	_angleXS = _angleX;
	_angleYS = _angleY;
}
