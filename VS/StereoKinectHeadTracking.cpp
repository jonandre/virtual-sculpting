#include "StereoKinectHeadTracking.h"
#include "NuiApi.h"

void StereoKinectHeadTracking::Fail (std::string s) {
	std::cout << "StereoKinectHeadTracking: " << s << std::endl;
	m_ready = false;
}

StereoKinectHeadTracking::StereoKinectHeadTracking() :
	m_ready(false)
{
	m_headPosition = new Vector4;
}

StereoKinectHeadTracking::~StereoKinectHeadTracking()
{
	delete m_headPosition;
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

			(*m_headPosition) = skel.SkeletonPositions[NUI_SKELETON_POSITION_HEAD];

			break; // we just want a skeleton
        }
    }
}

const Vector4* StereoKinectHeadTracking::GetHeadPosition() {
	std::cout << m_headPosition->x << ", " << m_headPosition->y << ", " << m_headPosition->z << ", " << m_headPosition->w << std::endl;
	return m_headPosition;
}