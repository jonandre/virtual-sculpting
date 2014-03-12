#pragma once

#include "Windows.h"
#include "glm\glm.hpp"  

struct INuiSensor;

#include <iostream>
#include <string>

class StereoKinectHeadTracking
{
public:
	StereoKinectHeadTracking();

	~StereoKinectHeadTracking();

	void Init(INuiSensor* sensor);

	void Update();

	glm::vec3 GetHeadPosition();

	void SetViewportSize(float w, float h);

	float GetWorldRatio();

private:
	float EYE_DISTANCE;
	float DISPLAY_RW_WIDTH;
	float DISPLAY_RW_HEIGHT;
	float VIEWPORT_WIDTH;
	float VIEWPORT_HEIGHT;
	float SENSOR_ANGLE;
	float SENSOR_RW_POS_X;
	float SENSOR_RW_POS_Y;
	float SENSOR_RW_POS_Z;

	float RW_TO_VW_RATIO;

	struct SensorRelPoint {
		LONG x;
		LONG y;
		USHORT z;
		glm::vec3 rwPos; // Real world coordinate
		glm::vec3 vwPos; // Virtual world coordinate
	};

	bool m_ready;

	SensorRelPoint m_headPosition;

	INuiSensor* m_pNuiSensor;

	HANDLE m_hNextSkeletonEvent;

	void Fail (std::string s);
};