#include "main.h"
#include <vector>



class GridModel;
class Input
{
public:
	Input();
	~Input();
	
	bool useHaptics;
	bool useSound;

	void OnKeyPressed( char c );
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

	void UpdateFrame();

	bool space_pressed;
	glm::vec3 _rotation_vector_obj;

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
	vector<char> _pressed_keys;

	GridModel* _model;
};

