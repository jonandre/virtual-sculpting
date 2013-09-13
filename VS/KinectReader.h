#pragma once
#include "Windows.h"
struct INuiSensor;
//#include "NuiImageCamera.h"
struct Point;

class KinectReader
{
public:
	KinectReader( unsigned int min_depth, unsigned int max_depth, float dist );
	~KinectReader();
	float* GetDepth();
	void ProcessDepth();
private:
	KinectReader();
	unsigned int _min_depth;
	unsigned int _max_depth;
	float        _active_depth;


	// Current Kinect
    INuiSensor*             m_pNuiSensor;
	static const int        cDepthWidth  = 640;
    static const int        cDepthHeight = 480;
    static const int        cBytesPerPixel = 1;



	float*					m_depth;

	HRESULT                 CreateFirstConnected();
	HANDLE                  m_hNextDepthFrameEvent;
	HANDLE                  m_pDepthStreamHandle;
};

