#pragma once
#pragma GCC diagnostic push
#pragma GCC system_header
#include <windows.h>
#pragma GCC diagnostic pop
#ifndef WITHOUT_KINECT
struct INuiSensor;
//#include <NuiImageCamera.h>
#endif
struct Point;


class KinectReader
{
public:
	KinectReader(unsigned int min_depth, unsigned int max_depth, float dist);
	~KinectReader();
	float* GetDepth();
	void ProcessDepth();
	
private:
	KinectReader();
	unsigned int _min_depth;
	unsigned int _max_depth;
	float _active_depth;
	
	// Current Kinect
	#ifndef WITHOUT_KINECT
	INuiSensor* m_pNuiSensor;
	#endif
	static const int cDepthWidth = 640;
	static const int cDepthHeight = 480;
	static const int cBytesPerPixel = 1;
	
	float* m_depth;
	
	HRESULT CreateFirstConnected();
	HANDLE m_hNextDepthFrameEvent;
	HANDLE m_pDepthStreamHandle;
};

