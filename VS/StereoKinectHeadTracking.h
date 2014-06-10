#ifndef STEREO_KINECT_HEAD_TRACKING_H
#define STEREO_KINECT_HEAD_TRACKING_H

#include "Windows.h"
#include "glm\glm.hpp"
#include "glm\gtc\matrix_transform.hpp"
#include "NuiApi.h"
#include <iostream>
#include <string>

class StereoKinectHeadTracking
{
public:
	StereoKinectHeadTracking();

	~StereoKinectHeadTracking();

	void Init(INuiSensor* sensor);

	void Update(float deltaTime);
	
	void RetrieveMatrices(glm::vec3 interestPoint, glm::mat4& leftProj, glm::mat4& leftEye, glm::mat4& rightProj, glm::mat4& rightEye);
	

	glm::vec3 GetHeadPosition();

	glm::vec3 GetHandPosition(bool left);

	glm::vec3 GetEyePosition(bool left);
	
	glm::vec2 GetRealToVirtualWorldRatio();
	
	glm::vec3 GetSensorOriginOnVirtualWorld();

	INuiSensor* GetSensor();
	

	void SetViewportSize(float w, float h);

	void SetDisplaySize(float w, float h);

	void SetZPlanes(float znear, float zfar);

	void SetEyeDistance(float eyeDistance);

	void SetHeadRadius(float headRadius);

	void SetInterestFacing (bool on);
	
	void SetSensorFloorAngle (float angle);

	void SetSensorPosition(float x, float y, float z);

	void SetHeatTracking (bool on);

	glm::vec3 SensorToVirtualWorldCoordinates(glm::vec3 sPos);
	
private:
	float RW_EYE_DISTANCE, EYE_DISTANCE;
	float RW_HEAD_RADIUS, HEAD_RADIUS;
	float DISPLAY_RW_WIDTH, DISPLAY_RW_HEIGHT;
	float VIEWPORT_WIDTH, VIEWPORT_HEIGHT;
	float SENSOR_ANGLE;
	float SENSOR_RW_POS_X, SENSOR_RW_POS_Y, SENSOR_RW_POS_Z;
	float ZNEAR, ZFAR;
	glm::vec2 RW_TO_VW_RATIO;
	bool FACE_INTEREST;

	bool firstTick;

	glm::vec3 interestPoint;

	class SensorRelPoint {
	public:
		glm::vec3 rwPos; // Real world coordinate
		glm::vec3 vwPos; // Virtual world coordinate
		NUI_SKELETON_POSITION_TRACKING_STATE state;
	};

	bool m_ready;
	float headDist;

	SensorRelPoint m_headPosition;
	SensorRelPoint m_leftHandPosition;
	SensorRelPoint m_rightHandPosition;

	INuiSensor* m_pNuiSensor;

	HANDLE m_hNextSkeletonEvent;

	void Fail (std::string s);	

	glm::vec3 vwSensorOrigin;

	void ViewportChanged();
};

#endif // STEREO_KINECT_HEAD_TRACKING_H