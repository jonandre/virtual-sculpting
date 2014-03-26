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

	glm::vec3 GetHeadPosition();

	void SetViewportSize(float w, float h);

	void SetDisplaySize(float w, float h);

	void SetZPlanes(float znear, float zfar);

	void SetEyeDistance(float eyeDistance);

	void SetScreemFacing (bool on);

	void SetSensorPosition(float x, float y, float z);

	void SetHeatTracking (bool on);

	float GetRealToVirtualWorldRatio();

	void RetrieveMatrices(glm::mat4& leftProj, glm::mat4& leftEye, glm::mat4& rightProj, glm::mat4& rightEye);

	void SetPredictionFactor(glm::vec3 factor);

private:
	float EYE_DISTANCE;
	float DISPLAY_RW_WIDTH, DISPLAY_RW_HEIGHT;
	float VIEWPORT_WIDTH, VIEWPORT_HEIGHT;
	float SENSOR_ANGLE;
	float SENSOR_RW_POS_X, SENSOR_RW_POS_Y, SENSOR_RW_POS_Z;
	float ZNEAR, ZFAR;
	float RW_TO_VW_RATIO;
	bool FACE_SCREEN;

	class SensorRelPoint {
	public:
		//LONG x;
		//LONG y;
		//USHORT z;
		glm::vec3 rwPos; // Real world coordinate
		glm::vec3 vwPos; // Virtual world coordinate

		// Prediction
		glm::vec3 predictionFactor;
		glm::vec3 lastRwPos;
		glm::vec3 vel;
		glm::vec3 acc;

		glm::vec3 Predict (float deltaTime)
		{
			glm::vec3 ret;
			ret.x = predictionFactor.x*(vel.x*deltaTime + 0.5*acc.x*deltaTime*deltaTime);
			ret.y = predictionFactor.y*(vel.y*deltaTime + 0.5*acc.y*deltaTime*deltaTime);
			ret.z = predictionFactor.z*(vel.z*deltaTime + 0.5*acc.z*deltaTime*deltaTime);

			return ret;
		}
	};

	bool m_ready;

	SensorRelPoint m_headPosition;

	INuiSensor* m_pNuiSensor;

	HANDLE m_hNextSkeletonEvent;

	void Fail (std::string s);

	
	
	
};