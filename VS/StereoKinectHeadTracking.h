#pragma once

#include "Windows.h"
#include "glm\glm.hpp"
#include "glm\gtc\matrix_transform.hpp"


struct INuiSensor;

#include <iostream>
#include <string>

class StereoKinectHeadTracking
{
public:
	StereoKinectHeadTracking();

	~StereoKinectHeadTracking();

	void Init(INuiSensor* sensor);

	void Update(float deltaTime);
	
	void RetrieveMatrices(glm::mat4& leftProj, glm::mat4& leftEye, glm::mat4& rightProj, glm::mat4& rightEye);
	

	glm::vec3 GetHeadPosition();
	
	float GetRealToVirtualWorldRatio();
	
	glm::vec3 GetSensorOriginOnVirtualWorld();
	

	void SetViewportSize(float w, float h);

	void SetDisplaySize(float w, float h);

	void SetZPlanes(float znear, float zfar);

	void SetEyeDistance(float eyeDistance);

	void SetScreenFacing (bool on);
	
	void SetSensorFloorAngle (float angle);

	void SetSensorPosition(float x, float y, float z);

	void SetHeatTracking (bool on);

	glm::vec3 SensorToVirtualWorldCoordinates(glm::vec3 sPos);
	
private:
	float EYE_DISTANCE;
	float DISPLAY_RW_WIDTH, DISPLAY_RW_HEIGHT;
	float VIEWPORT_WIDTH, VIEWPORT_HEIGHT;
	float SENSOR_ANGLE;
	float SENSOR_RW_POS_X, SENSOR_RW_POS_Y, SENSOR_RW_POS_Z;
	float ZNEAR, ZFAR;
	float RW_TO_VW_RATIO;
	bool FACE_SCREEN;

	bool firstTick;

	class SensorRelPoint {
	public:
		glm::vec3 rwPos; // Real world coordinate
		glm::vec3 vwPos; // Virtual world coordinate
	};

	bool m_ready;

	SensorRelPoint m_headPosition;

	INuiSensor* m_pNuiSensor;

	HANDLE m_hNextSkeletonEvent;

	void Fail (std::string s);	

	glm::vec3 vwSensorOrigin;
	
	glm::vec3 RealWorldToSensorCoordinates(glm::vec3 rwPos);
};