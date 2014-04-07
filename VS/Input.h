#ifndef _INPUT_HPP_
#define _INPUT_HPP_

#include "main.h"
#include "SDL.h"
#include <vector>

#include "GridModel.h"

class Input
{
public:
	Input();
	~Input();


	void Back(void);


	
	void OnKeyPressed( SDL_Keycode c );
	void OnMouseLBDown( int x, int y );
	void OnMouseMove( int dx, int dy );
	void OnMouseLBUp( int x, int y );
	bool IsPressed( char c );
	void OnSroll( int dx );

	glm::mat4 GetObjectM();
	glm::quat GetObjectQ();
	glm::mat4 GetViewM();

	void SetViewM(const glm::mat4& mat );
	void SetZoom( float val );
	void SetModel( GridModel* );

	float GetZoom ();

	void UpdateFrame();

	glm::vec3 _rotation_vector_obj;
	
	GridModel* _model;

	void newGridModel(bool);

	void rotateYGridModel(int);
	void rotateXGridModel(int);
	void rotateZGridModel(int);

	void rotateFaster();
	void rotateSlower();

	void freezGridModel();
	void clearRotationGridModel();

	void rotate90DegreesRightY();
	void rotate90DegreesLeftY();

	void rotateTopView();
	void rotateRightView();
	void rotateLeftView();
	void rotateBottomView();
	void rotateBackView();

	void cameraRotationViewFront();
	void cameraRotationViewBack();
	void cameraRotationViewTop();
	void cameraRotationViewBottom();
	void cameraRotationViewLeft();
	void cameraRotationViewRight();

	void Quit();
	

private:
	int _mouseStartX;
	int _mouseStartY;
	int _prevX;
	int _prevY;
	bool _lbtn_pressed;
	float _angleXS;
	float _angleX;
    float _angleYS;
	float _angleY;
	//float _rotationSpeedValu;
	//glm::mat4 _obj_mat;
	glm::quat _obj_quat;
	glm::mat4 _view_mat;
	float zoom_val;

	
	int rx;
	int ry;
	int rz;

};

#endif

