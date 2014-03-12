#pragma once

#include "Windows.h"

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

	const Vector4* GetHeadPosition();

private:
	bool m_ready;

	Vector4* m_headPosition;

	INuiSensor* m_pNuiSensor;

	HANDLE m_hNextSkeletonEvent;

	void Fail (std::string s);
};