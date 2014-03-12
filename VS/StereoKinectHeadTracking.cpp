#include "StereoKinectHeadTracking.h"
#include "NuiApi.h"

#define DEG_TO_RAD ((2.0*3.14159265358979323846264338327950288) / 360.0)

void StereoKinectHeadTracking::Fail (std::string s) {
	std::cout << "StereoKinectHeadTracking: " << s << std::endl;
	m_ready = false;
}

StereoKinectHeadTracking::StereoKinectHeadTracking() :
	m_ready(false)
{
	EYE_DISTANCE = 0.065f;
	DISPLAY_RW_WIDTH = 4.0055f;
	DISPLAY_RW_HEIGHT = 2.430f;
	VIEWPORT_WIDTH = 1.0f;
	VIEWPORT_HEIGHT = 1.0f;
	// Relative to the horizontal plane (the floor)
	SENSOR_ANGLE = 20.0f;
	// Relative to the center of the screen, in meters
	SENSOR_RW_POS_X = 0.0f;
	SENSOR_RW_POS_Y = DISPLAY_RW_HEIGHT/2.0f - 0.40f;
	SENSOR_RW_POS_Z = 0.2f;
}

StereoKinectHeadTracking::~StereoKinectHeadTracking()
{
	if (m_hNextSkeletonEvent && (m_hNextSkeletonEvent != INVALID_HANDLE_VALUE))
    {
        CloseHandle(m_hNextSkeletonEvent);
    }
}

void StereoKinectHeadTracking::Init(INuiSensor* sensor)
{
	m_pNuiSensor = sensor;

	HRESULT hr;

	// Create an event that will be signaled when skeleton data is available
    m_hNextSkeletonEvent = CreateEventW(NULL, TRUE, FALSE, NULL);

    // Open a skeleton stream to receive skeleton data
    hr = m_pNuiSensor->NuiSkeletonTrackingEnable(m_hNextSkeletonEvent, 0);

	if (FAILED(hr))
	{
		Fail("Failed to initialize skeleton tracking on Sensor");
	}

	m_ready = true;
}

void StereoKinectHeadTracking::Update()
{
	if (!m_ready) return;

	 // Wait for 0ms, just quickly test if it is time to process a skeleton
    if ( WAIT_OBJECT_0 != WaitForSingleObject(m_hNextSkeletonEvent, 0) ) return;

	NUI_SKELETON_FRAME skeletonFrame = {0};

    HRESULT hr = m_pNuiSensor->NuiSkeletonGetNextFrame(0, &skeletonFrame);
    if ( FAILED(hr) )
    {
        return;
    }

    // smooth out the skeleton data
    m_pNuiSensor->NuiTransformSmooth(&skeletonFrame, NULL);
	
    for (int i = 0 ; i < NUI_SKELETON_COUNT; ++i)
    {
		const NUI_SKELETON_DATA& skel = skeletonFrame.SkeletonData[i];
        NUI_SKELETON_TRACKING_STATE trackingState = skel.eTrackingState;

        if (trackingState == NUI_SKELETON_TRACKED)
        {
            // We're tracking the skeleton, draw it
			NUI_SKELETON_POSITION_TRACKING_STATE headState = skel.eSkeletonPositionTrackingState[NUI_SKELETON_POSITION_HEAD];

			if (headState == NUI_SKELETON_POSITION_NOT_TRACKED) continue; // We don't have the head of this skeleton

			NuiTransformSkeletonToDepthImage(skel.SkeletonPositions[NUI_SKELETON_POSITION_HEAD], &m_headPosition.x, &m_headPosition.y, &m_headPosition.z);

			m_headPosition.rwPos.x = float(int(m_headPosition.x) - 150)/300.0f;
			m_headPosition.rwPos.y = float(int(m_headPosition.y) - 100)*(-1.0f)/200.0f;
			m_headPosition.rwPos.z = float(m_headPosition.z >> 3)  / 1000.0f - 2.1f; // the position where is the 0.0

			m_headPosition.vwPos.x = (SENSOR_RW_POS_X + m_headPosition.rwPos.x) * RW_TO_VW_RATIO;
			m_headPosition.vwPos.y = (SENSOR_RW_POS_Y + m_headPosition.rwPos.y/glm::cos(SENSOR_ANGLE*DEG_TO_RAD) + m_headPosition.rwPos.z/glm::sin(SENSOR_ANGLE*DEG_TO_RAD)) * RW_TO_VW_RATIO;
			m_headPosition.vwPos.z = (SENSOR_RW_POS_Z - m_headPosition.rwPos.y/glm::sin(SENSOR_ANGLE*DEG_TO_RAD) + m_headPosition.rwPos.z/glm::cos(SENSOR_ANGLE*DEG_TO_RAD)) * RW_TO_VW_RATIO;

			break; // we just want a skeleton
        }
    }
}

glm::vec3 StereoKinectHeadTracking::GetHeadPosition() {
	if (false)
		std::cout << m_headPosition.vwPos.x << ", " << m_headPosition.vwPos.y << ", " << m_headPosition.rwPos.z << std::endl;
	return m_headPosition.vwPos;
}

void StereoKinectHeadTracking::SetViewportSize(float w, float h) {
	VIEWPORT_WIDTH = w;
	VIEWPORT_HEIGHT = h;

	RW_TO_VW_RATIO = VIEWPORT_HEIGHT / DISPLAY_RW_HEIGHT;
}

float StereoKinectHeadTracking::GetWorldRatio() {
	return RW_TO_VW_RATIO;
}