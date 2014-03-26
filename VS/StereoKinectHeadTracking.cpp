#include "StereoKinectHeadTracking.h"
#include "NuiApi.h"

#define DEG_TO_RAD ((2.0*3.14159265358979323846264338327950288) / 360.0)

void StereoKinectHeadTracking::Fail (std::string s) {
	std::cout << "StereoKinectHeadTracking: " << s << std::endl;
	m_ready = false;
}

StereoKinectHeadTracking::StereoKinectHeadTracking() :
	m_ready(false), FACE_SCREEN(false)
{
	EYE_DISTANCE = 0.065f;
	DISPLAY_RW_WIDTH = 4.0055f;
	DISPLAY_RW_HEIGHT = 2.430f;
	VIEWPORT_WIDTH = 1.0f;
	VIEWPORT_HEIGHT = 1.0f;
}

StereoKinectHeadTracking::~StereoKinectHeadTracking ()
{
	if (m_hNextSkeletonEvent && (m_hNextSkeletonEvent != INVALID_HANDLE_VALUE))
    {
        CloseHandle(m_hNextSkeletonEvent);
    }
}

void StereoKinectHeadTracking::Init (INuiSensor* sensor)
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

	long angle;
	sensor->NuiCameraElevationGetAngle(&angle);
	SENSOR_ANGLE = float(angle);

	m_ready = true;
}

void StereoKinectHeadTracking::Update (float deltaTime)
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

			if (headState == NUI_SKELETON_POSITION_NOT_TRACKED
				//|| headState == NUI_SKELETON_POSITION_INFERRED
				) continue; // We don't have the head of this skeleton

			//NuiTransformSkeletonToDepthImage(skel.SkeletonPositions[NUI_SKELETON_POSITION_HEAD], &m_headPosition.x, &m_headPosition.y, &m_headPosition.z);

			// Real world coordinates transform (in meters)
			m_headPosition.lastRwPos = m_headPosition.rwPos;

			m_headPosition.rwPos.x = skel.SkeletonPositions[NUI_SKELETON_POSITION_HEAD].x;
			m_headPosition.rwPos.y = skel.SkeletonPositions[NUI_SKELETON_POSITION_HEAD].y;
			m_headPosition.rwPos.z = skel.SkeletonPositions[NUI_SKELETON_POSITION_HEAD].z;

			glm::vec3 vel, acc;
			vel = (m_headPosition.rwPos - m_headPosition.lastRwPos) / deltaTime;
			acc = (vel - m_headPosition.vel) / deltaTime;

			// Smoothing factor
			float sf = 0.7f;
			m_headPosition.vel = m_headPosition.vel*sf + vel*(1.0f - sf);
			m_headPosition.acc = m_headPosition.acc*sf + vel*(1.0f - sf);

			glm::vec3 pred = m_headPosition.Predict(deltaTime);
			// Virtual world
			m_headPosition.vwPos.x = (SENSOR_RW_POS_X + m_headPosition.rwPos.x + pred.x) * RW_TO_VW_RATIO;
			m_headPosition.vwPos.y = (SENSOR_RW_POS_Y + m_headPosition.rwPos.y*glm::cos(SENSOR_ANGLE*DEG_TO_RAD) + m_headPosition.rwPos.z*glm::sin(SENSOR_ANGLE*DEG_TO_RAD) + pred.y) * RW_TO_VW_RATIO;
			m_headPosition.vwPos.z = (SENSOR_RW_POS_Z - m_headPosition.rwPos.y*glm::sin(SENSOR_ANGLE*DEG_TO_RAD) + m_headPosition.rwPos.z*glm::cos(SENSOR_ANGLE*DEG_TO_RAD) + pred.z) * RW_TO_VW_RATIO;

			break; // we just want a skeleton
        }
    }
}

glm::vec3 StereoKinectHeadTracking::GetHeadPosition()
{
	if (!m_ready) return glm::vec3(0.0f);
	else return m_headPosition.vwPos;
}

void StereoKinectHeadTracking::SetViewportSize (float w, float h) {
	VIEWPORT_WIDTH = w;
	VIEWPORT_HEIGHT = h;

	RW_TO_VW_RATIO = VIEWPORT_HEIGHT / DISPLAY_RW_HEIGHT;
}

void StereoKinectHeadTracking::SetDisplaySize (float w, float h)
{
	DISPLAY_RW_WIDTH = w;
	DISPLAY_RW_HEIGHT = h;
	
	RW_TO_VW_RATIO = VIEWPORT_HEIGHT / DISPLAY_RW_HEIGHT;
}

void StereoKinectHeadTracking::SetZPlanes (float znear, float zfar)
{
	ZNEAR = znear;
	ZFAR = zfar;
}

void StereoKinectHeadTracking::SetEyeDistance(float eyeDistance)
{
	EYE_DISTANCE = eyeDistance;
}

void StereoKinectHeadTracking::SetSensorPosition(float x, float y, float z)
{
	SENSOR_RW_POS_X = x;
	SENSOR_RW_POS_Y = y;
	SENSOR_RW_POS_Z = z;
}

float StereoKinectHeadTracking::GetRealToVirtualWorldRatio()
{
	return RW_TO_VW_RATIO;
}

void StereoKinectHeadTracking::SetScreemFacing (bool on)
{
	FACE_SCREEN = on;
}

void StereoKinectHeadTracking::SetHeatTracking (bool on)
{
	m_ready = on;
}

void StereoKinectHeadTracking::RetrieveMatrices(glm::mat4& leftProj, glm::mat4& leftEye, glm::mat4& rightProj, glm::mat4& rightEye)
{
	glm::vec3 head = GetHeadPosition();
	
	float focus, ndfl;
	float tf = (VIEWPORT_HEIGHT/2.0 - head.y) / VIEWPORT_HEIGHT;
	float bf = (head.y - (-VIEWPORT_HEIGHT/2.0)) / VIEWPORT_HEIGHT;
	float rf, lf;

	float left, right, top, bottom;

	float headRightFactorX = 1.0f;
	float headRightFactorZ = 0.0f;
	// If we are facing the screen we should evaluate the angle to the middle of the screen
	if (FACE_SCREEN) {
		glm::vec2 aux (head.z, -head.x);
		aux = glm::normalize(aux);
		headRightFactorZ = aux.y;
		headRightFactorX = aux.x;

		std::cout << headRightFactorX << ", " << headRightFactorZ << std::endl;
	}

	//Left eye
	focus = head.z - 0.5 * EYE_DISTANCE * headRightFactorZ;
	ndfl = ZNEAR / focus;
	
	top = VIEWPORT_HEIGHT * ndfl * tf;
	bottom = -VIEWPORT_HEIGHT * ndfl * bf;

	rf = (VIEWPORT_WIDTH/2.0 - (head.x - 0.5 * EYE_DISTANCE * headRightFactorX)) / VIEWPORT_WIDTH;
	lf = ((head.x - 0.5 * EYE_DISTANCE * headRightFactorX) - (-VIEWPORT_WIDTH/2.0)) / VIEWPORT_WIDTH;

	left = -VIEWPORT_WIDTH * ndfl * lf;
	right = VIEWPORT_WIDTH * ndfl * rf;

	leftProj = glm::frustum(left, right, bottom, top, ZNEAR, ZFAR);
	leftEye = glm::translate(glm::mat4(1.0f), -head);
	leftEye = glm::translate(leftEye, glm::vec3(headRightFactorX,0,headRightFactorZ)*(EYE_DISTANCE/2.0f));

	//Right eye
	focus = head.z + 0.5 * EYE_DISTANCE * headRightFactorZ;
	ndfl = ZNEAR / focus;
	
	top = VIEWPORT_HEIGHT * ndfl * tf;
	bottom = -VIEWPORT_HEIGHT * ndfl * bf;

	rf = (VIEWPORT_WIDTH/2.0 - (head.x + 0.5 * EYE_DISTANCE * headRightFactorX)) / VIEWPORT_WIDTH;
	lf = ((head.x + 0.5 * EYE_DISTANCE * headRightFactorX) - (-VIEWPORT_WIDTH/2.0)) / VIEWPORT_WIDTH;

	left = -VIEWPORT_WIDTH * ndfl * lf;
	right = VIEWPORT_WIDTH * ndfl * rf;

	rightProj = glm::frustum(left, right, bottom, top, ZNEAR, ZFAR);
	rightEye = glm::translate(glm::mat4(1.0f), -head);
	rightEye = glm::translate(rightEye, -glm::vec3(headRightFactorX,0,headRightFactorZ)*(EYE_DISTANCE/2.0f));
}

void StereoKinectHeadTracking::SetPredictionFactor(glm::vec3 factor)
{
	m_headPosition.predictionFactor = factor;
}