#ifndef _INPUT_HPP_
#define _INPUT_HPP_

#include "main.h"
#include "SDL.h"
#include "STLExporter.h"
#include "DataExporter.h"
#include <vector>

#include "GridModel.h"

class Input
{
public:
	Input();
	~Input();


	void Back(void);
	
	void OnKeyPressed( SDL_Keycode c );
	void OnKeyReleased( SDL_Keycode c );
	void OnMouseLBDown( int x, int y );
	void OnMouseMove( int dx, int dy );
	void OnMouseLBUp( int x, int y );
	bool IsPressed( char c );
	void OnSroll( int dx );

	glm::mat4 GetObjectM();
	glm::quat GetObjectQ();
	glm::mat4 GetViewM();
	glm::mat4 GetModelM();

	void SetViewM(const glm::mat4& mat );
	void SetZoom( float val );
	void SetModel( GridModel* );
	void SetModelSide(float wantedSide);
	void SetModelPosition(glm::vec3 pos);
	
	void SetDataExporter(DataExporter* data);

	glm::vec3 GetObjectPosition();

	float GetZoom ();
	float GetModelSide();

	void UpdateFrame(float deltaTime);

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
	
	void TranslateGridModel(glm::vec3& translation);
	glm::vec3 GetRotationFromTo(glm::vec3& from, glm::vec3& to);

	void Quit();

	void UpdateHandPosition(glm::vec3& lHand, glm::vec3& rHand);
	

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
	glm::vec3 rotSpeed;
	glm::vec3 _obj_pos;
	float _obj_scale;
	glm::mat4 _view_mat;
	float zoom_val;

	float wantedScale;
		float wantedSide;
	glm::vec3 wantedPos;

	bool rotateOn;
	glm::vec3 handPosition;
	glm::vec3 lastHandPosition;
	glm::vec3 handVelocity;
	
	int rx;
	int ry;
	int rz;

	DataExporter* _dataExporter;
};

#endif

