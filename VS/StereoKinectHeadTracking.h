#pragma once

#include "NuiApi.h"
#include <iostream>
#include <string>

class StereoKinectHeadTracking
{
public:
	StereoKinectHeadTracking();

	~StereoKinectHeadTracking();

	void Init(INuiSensor* sensor);

	void Update();

private:
	bool m_ready;

	INuiSensor* m_pNuiSensor;

	HANDLE m_hNextSkeletonEvent;

	void Fail (std::string s);
};