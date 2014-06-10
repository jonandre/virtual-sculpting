#include "StereoKinectHeadTracking.h"

#define DEG_TO_RAD ((2.0*3.14159265358979323846264338327950288) / 360.0)

void StereoKinectHeadTracking::Fail (std::string s) {
	std::cout << "StereoKinectHeadTracking: " << s << std::endl;
	m_ready = false;
}

// Creator
StereoKinectHeadTracking::StereoKinectHeadTracking() :
	m_ready(false), FACE_INTEREST(false)
{
	EYE_DISTANCE = 0.065f;
	DISPLAY_RW_WIDTH = 4.0055f;
	DISPLAY_RW_HEIGHT = 2.430f;
	VIEWPORT_WIDTH = 1.0f;
	VIEWPORT_HEIGHT = 1.0f;

	firstTick = true;
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
	if (firstTick) {
		vwSensorOrigin = SensorToVirtualWorldCoordinates(glm::vec3(0.0f));
		m_headPosition.vwPos = vwSensorOrigin;
		firstTick = false;
	}

	if (!m_ready) {
		return;
	}

	 // Wait for 0ms, just quickly test if it is time to process a skeleton
    if ( WAIT_OBJECT_0 != WaitForSingleObject(m_hNextSkeletonEvent, 0) ) return;

	NUI_SKELETON_FRAME skeletonFrame = {0};

    HRESULT hr = m_pNuiSensor->NuiSkeletonGetNextFrame(0, &skeletonFrame);
    if ( FAILED(hr) )
    {
        return;
    }

//	const NUI_TRANSFORM_SMOOTH_PARAMETERS SMOOTHING_PARAMS = 
//		{0.1f, 0.7f, 1.0f, 0.025f, 0.02f};
	const NUI_TRANSFORM_SMOOTH_PARAMETERS SMOOTHING_PARAMS = 
		{0.5f, 0.7f, 0.5f, 0.05f, 0.04f};

    // smooth out the skeleton data
    m_pNuiSensor->NuiTransformSmooth(&skeletonFrame, &SMOOTHING_PARAMS);
	
	headDist = 999999.0f;
    for (int i = 0 ; i < NUI_SKELETON_COUNT; ++i)
    {
		const NUI_SKELETON_DATA& skel = skeletonFrame.SkeletonData[i];
        NUI_SKELETON_TRACKING_STATE trackingState = skel.eTrackingState;

        if (trackingState == NUI_SKELETON_TRACKED)
        {
			NUI_SKELETON_POSITION_TRACKING_STATE headState = skel.eSkeletonPositionTrackingState[NUI_SKELETON_POSITION_HEAD];

			if (headState == NUI_SKELETON_POSITION_NOT_TRACKED)
				continue;

			// Real world coordinates transform (in meters)
			glm::vec3 aux;
			aux.x = skel.SkeletonPositions[NUI_SKELETON_POSITION_HEAD].x;
			aux.y = skel.SkeletonPositions[NUI_SKELETON_POSITION_HEAD].y;
			aux.z = skel.SkeletonPositions[NUI_SKELETON_POSITION_HEAD].z;

			if (glm::length(aux) > headDist) continue;
			headDist = glm::length(aux);

			//if (headState != NUI_SKELETON_POSITION_INFERRED)// If the position is inferred we don't want to trust it
				m_headPosition.rwPos = aux;

			m_headPosition.vwPos = SensorToVirtualWorldCoordinates(m_headPosition.rwPos);


			// HANDS
			glm::vec3 lHand;
			lHand.x = skel.SkeletonPositions[NUI_SKELETON_POSITION_HAND_LEFT].x;
			lHand.y = skel.SkeletonPositions[NUI_SKELETON_POSITION_HAND_LEFT].y;
			lHand.z = skel.SkeletonPositions[NUI_SKELETON_POSITION_HAND_LEFT].z;

			m_leftHandPosition.rwPos = lHand;

			m_leftHandPosition.vwPos = SensorToVirtualWorldCoordinates(m_leftHandPosition.rwPos);

			glm::vec3 rHand;
			rHand.x = skel.SkeletonPositions[NUI_SKELETON_POSITION_HAND_RIGHT].x;
			rHand.y = skel.SkeletonPositions[NUI_SKELETON_POSITION_HAND_RIGHT].y;
			rHand.z = skel.SkeletonPositions[NUI_SKELETON_POSITION_HAND_RIGHT].z;

			m_rightHandPosition.rwPos = rHand;

			m_rightHandPosition.vwPos = SensorToVirtualWorldCoordinates(m_rightHandPosition.rwPos);
        }
    }
}

glm::vec3 StereoKinectHeadTracking::SensorToVirtualWorldCoordinates(glm::vec3 sPos) {
	glm::vec3 vwPos;

	/*sPos.y *= 0.85f;
	sPos.x *= 0.77f;
	sPos.z *= 1.1f;*/

	vwPos.x = (SENSOR_RW_POS_X + sPos.x) * RW_TO_VW_RATIO.x;
	vwPos.y = (SENSOR_RW_POS_Y + sPos.y*glm::cos(SENSOR_ANGLE*DEG_TO_RAD) + sPos.z*glm::sin(SENSOR_ANGLE*DEG_TO_RAD)) * RW_TO_VW_RATIO.x;
	vwPos.z = (SENSOR_RW_POS_Z - sPos.y*glm::sin(SENSOR_ANGLE*DEG_TO_RAD) + sPos.z*glm::cos(SENSOR_ANGLE*DEG_TO_RAD)) * RW_TO_VW_RATIO.x;

	return vwPos;
}

glm::vec3 StereoKinectHeadTracking::GetHeadPosition()
{
	return m_headPosition.vwPos;
}

glm::vec3 StereoKinectHeadTracking::GetHandPosition(bool left)
{
	return left? m_leftHandPosition.vwPos : m_rightHandPosition.vwPos;
}

void StereoKinectHeadTracking::SetViewportSize (float w, float h) {
	VIEWPORT_WIDTH = w;
	VIEWPORT_HEIGHT = h;

	ViewportChanged();
}

void StereoKinectHeadTracking::SetDisplaySize (float w, float h)
{
	DISPLAY_RW_WIDTH = w;
	DISPLAY_RW_HEIGHT = h;
	
	ViewportChanged();
}

void StereoKinectHeadTracking::SetSensorFloorAngle (float angle) 
{
	SENSOR_ANGLE += angle;
}

void StereoKinectHeadTracking::SetZPlanes (float znear, float zfar)
{
	ZNEAR = znear;
	ZFAR = zfar;
}

void StereoKinectHeadTracking::SetEyeDistance(float eyeDistance)
{
	RW_EYE_DISTANCE = eyeDistance;
	EYE_DISTANCE = eyeDistance;
}

void StereoKinectHeadTracking::SetHeadRadius(float headRadius)
{
	RW_HEAD_RADIUS = headRadius;
	HEAD_RADIUS = headRadius;
}

void StereoKinectHeadTracking::SetSensorPosition(float x, float y, float z)
{
	SENSOR_RW_POS_X = x;
	SENSOR_RW_POS_Y = y;
	SENSOR_RW_POS_Z = z;
}

glm::vec2 StereoKinectHeadTracking::GetRealToVirtualWorldRatio()
{
	return RW_TO_VW_RATIO;
}

void StereoKinectHeadTracking::SetInterestFacing (bool on)
{
	FACE_INTEREST = on;
}

void StereoKinectHeadTracking::SetHeatTracking (bool on)
{
	m_ready = on;
}

glm::vec3 StereoKinectHeadTracking::GetEyePosition(bool left)
{
	glm::vec3 head = GetHeadPosition();

	float headRightFactorX = 1.0f;
	float headRightFactorZ = 0.0f;
	// If we are facing the screen we should evaluate the angle to the middle of the screen
	if (FACE_INTEREST) {
		glm::vec3 interestDir = head - interestPoint; 
		glm::vec2 aux (interestDir.z, -interestDir.x);
		aux = glm::normalize(aux);
		headRightFactorZ = aux.y;
		headRightFactorX = aux.x;
	}

	glm::vec3 eye = ((left)? -1.0f : 1.0f)* glm::vec3(headRightFactorX,0,headRightFactorZ)*(EYE_DISTANCE/2.0f)
					- glm::vec3(-headRightFactorZ,0,headRightFactorX)*HEAD_RADIUS + head;
	return eye;
}

void StereoKinectHeadTracking::RetrieveMatrices(glm::vec3 interestPoint, glm::mat4& leftProj, glm::mat4& leftEye, glm::mat4& rightProj, glm::mat4& rightEye)
{
	this->interestPoint = interestPoint;

	glm::vec3 head = GetHeadPosition();
	
	float focus, ndfl;
	float tf = (VIEWPORT_HEIGHT/2.0 - head.y) / VIEWPORT_HEIGHT;
	float bf = (head.y - (-VIEWPORT_HEIGHT/2.0)) / VIEWPORT_HEIGHT;
	float rf, lf;

	float left, right, top, bottom;

	glm::vec3 eyePos;
	
	//Left eye
	eyePos = GetEyePosition(true);

	focus = eyePos.z;
	ndfl = ZNEAR / focus;
	
	top = VIEWPORT_HEIGHT * ndfl * tf;
	bottom = -VIEWPORT_HEIGHT * ndfl * bf;

	rf = (VIEWPORT_WIDTH/2.0 - eyePos.x) / VIEWPORT_WIDTH;
	lf = 1.0f - rf;

	left = -VIEWPORT_WIDTH * ndfl * lf;
	right = VIEWPORT_WIDTH * ndfl * rf;

	leftProj = glm::frustum(left, right, bottom, top, ZNEAR, ZFAR);
	leftEye = glm::translate(glm::mat4(1.0f), -eyePos + glm::vec3(0.0f, 0.0f, 0.0f)); // debug

	//Right eye
	eyePos = GetEyePosition(false);

	focus = eyePos.z;
	ndfl = ZNEAR / focus;
	
	top = VIEWPORT_HEIGHT * ndfl * tf;
	bottom = -VIEWPORT_HEIGHT * ndfl * bf;

	rf = (VIEWPORT_WIDTH/2.0 - eyePos.x) / VIEWPORT_WIDTH;
	lf = 1.0f - rf;

	left = -VIEWPORT_WIDTH * ndfl * lf;
	right = VIEWPORT_WIDTH * ndfl * rf;

	rightProj = glm::frustum(left, right, bottom, top, ZNEAR, ZFAR);
	rightEye = glm::translate(glm::mat4(1.0f), -eyePos + glm::vec3(0.0f, 0.0f, 0.0f)); // debug);
}

glm::vec3 StereoKinectHeadTracking::GetSensorOriginOnVirtualWorld()
{
	return vwSensorOrigin;
}

INuiSensor* StereoKinectHeadTracking::GetSensor()
{
	return m_pNuiSensor;
}

void StereoKinectHeadTracking::ViewportChanged()
{
	RW_TO_VW_RATIO.y = VIEWPORT_HEIGHT / DISPLAY_RW_HEIGHT;
	RW_TO_VW_RATIO.x = VIEWPORT_WIDTH / DISPLAY_RW_WIDTH;
	EYE_DISTANCE = RW_EYE_DISTANCE * RW_TO_VW_RATIO.x;
	HEAD_RADIUS = RW_HEAD_RADIUS * RW_TO_VW_RATIO.x;

	std::cout << "RW_TO_VW_RATIO is: " << RW_TO_VW_RATIO.x << ", " << RW_TO_VW_RATIO.y << std::endl;
}