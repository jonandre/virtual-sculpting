#include "KinectReader.h"

#include "main.h"

// For Kinect SDK APIs
/* Voice Recognition Start */
#include "stdafx.h"
#include <guiddef.h>

#define INITGUID

// Static initializers
LPCWSTR KinectReader::GrammarFileName = L"SpeechCommands.grxml";

/* Voice Recognition End */

KinectReader::KinectReader(void)
{
	
}

KinectReader::KinectReader( unsigned int min_depth, unsigned int max_depth, float dist):
    m_pKinectAudioStream(NULL),
    m_pSpeechStream(NULL),
    m_pSpeechRecognizer(NULL),
    m_pSpeechContext(NULL),
    m_pSpeechGrammar(NULL),
    m_hSpeechEvent(INVALID_HANDLE_VALUE)
{
	/*  Must be the array of pointers
		memset(b, c, l) set the l bytes starting at address b to the value c
	*/
	m_depth = new float[640*480];
	memset(m_depth, 0, 640*480*sizeof(float));

	/* Intilizes the Nui sensor */
	m_pNuiSensor = NULL;
	
	/* Sets this varibales */
	_min_depth = min_depth;
	_max_depth = max_depth;
	_active_depth = dist;

	/* Initilizes when first connected */ 
	//CreateFirstConnected();
}

/**
 * Deteaches the kinect sensor and speech recognicer including:
 * KinectAudioStream
 * SpeechStream
 * SpeechRecognizer
 * SpeechContext
 * SpeechGrammar
 */
KinectReader::~KinectReader(void)
{
	if (m_pNuiSensor)
    {
        m_pNuiSensor->NuiShutdown();

		if (m_hNextDepthFrameEvent != INVALID_HANDLE_VALUE)
		{
			CloseHandle(m_hNextDepthFrameEvent);
		}
    }
   // SafeRelease(m_pNuiSensor);
	delete [] m_depth;
	/* Voice Recognition Start */
    SafeRelease(m_pKinectAudioStream);
    SafeRelease(m_pSpeechStream);
    SafeRelease(m_pSpeechRecognizer);
    SafeRelease(m_pSpeechContext);
    SafeRelease(m_pSpeechGrammar);
	/* Voice Recognition End */
}

void KinectReader::Init(StereoKinectHeadTracking* headTracking)
{
	CreateFirstConnected(headTracking);
}

/// <summary>
/// Create the first connected Kinect found.
/// </summary>
/// <returns>S_OK on success, otherwise failure code.</returns>
HRESULT KinectReader::CreateFirstConnected(StereoKinectHeadTracking* headTracking)
{
    INuiSensor * pNuiSensor = NULL;
    HRESULT hr;
	
	m_headTracking = headTracking;

    int iSensorCount = 0;
    hr = NuiGetSensorCount(&iSensorCount);

    if (FAILED(hr))
    {
        return hr;
    }

    // Look at each Kinect sensor
    for (int i = 0; i < iSensorCount; ++i)
    {
        // Create the sensor so we can check status, if we can't create it, move on to the next
        hr = NuiCreateSensorByIndex(i, &pNuiSensor);
        if (FAILED(hr))
            continue;
        // Get the status of the sensor, and if connected, then we can initialize it
        hr = pNuiSensor->NuiStatus();
        if (S_OK == hr)
        {
            m_pNuiSensor = pNuiSensor;
            break;
        }
        // This sensor wasn't OK, so release it since we're not using it
        pNuiSensor->Release();
    }

    if (NULL != m_pNuiSensor)
    {
		// Initialize the Kinect and specify that we'll be using audio signal
        // Initialize the Kinect and specify that we'll be using depth
        hr = m_pNuiSensor->NuiInitialize(NUI_INITIALIZE_FLAG_USES_DEPTH | NUI_INITIALIZE_FLAG_USES_AUDIO | NUI_INITIALIZE_FLAG_USES_SKELETON); 
		
        if (SUCCEEDED(hr))
        {
            // Create an event that will be signaled when depth data is available
            m_hNextDepthFrameEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

            // Open a depth image stream to receive depth frames
            hr = m_pNuiSensor->NuiImageStreamOpen(
                NUI_IMAGE_TYPE_DEPTH,
                NUI_IMAGE_RESOLUTION_640x480,
                0,
                2,
                m_hNextDepthFrameEvent,
                &m_pDepthStreamHandle);

			/* Installation of skelleton traking begins here*/
			headTracking->Init(m_pNuiSensor);
			/* Installation of skelleton traking ends here*/
        }
    }

    if (NULL == m_pNuiSensor || FAILED(hr))
    {
		std::cout<<"No ready Kinect found!"<<std::endl;
		m_pNuiSensor = NULL;
        //SetStatusMessage(L"No ready Kinect found!");
        return E_FAIL;
    }
	
	printf("Kinect found. \n");
	
	// Done properly	
	m_depthVector.resize(cDepthWidth*cDepthHeight);
	
	for (unsigned int i = 0; i < m_depthVector.size(); ++i) {
		m_depthVector[i].x = i%cDepthWidth;
		m_depthVector[i].y = i/cDepthWidth;
		m_depthVector[i].depth = 255;
	}
	// Done properly

	/* Speech start */

	hr = InitializeAudioStream();
	if (FAILED(hr))
	{
		printf("Could not initialize audio stream.\n");
		return hr;
	}
	printf("Initialize Audio Stream... \n");
	
	hr = CreateSpeechRecognizer();
	if (FAILED(hr))
    {
        printf("Could not create speech recognizer. Please ensure that Microsoft Speech SDK and other sample requirements are installed.\n");
        return hr;
    }
	printf("Create Speech Recognizer... \n");

	hr = LoadSpeechGrammar();
	if (FAILED(hr))
    {
        printf("Could not load speech grammar. Please ensure that grammar configuration file was properly deployed.\n");
        return hr;
    }
	printf("Start Speech Recognition... \n");

	hr = StartSpeechRecognition();
    if (FAILED(hr))
    {
        printf("Could not start recognizing speech... \n");
        return hr;
    }
	/* Speech end */
    return hr;
}

void KinectReader::ProcessDepth()
{
	/* If no kinect*/
	if(!m_pNuiSensor)
		return;

    HRESULT hr;
    NUI_IMAGE_FRAME imageFrame;

    // Attempt to get the depth frame
    hr = m_pNuiSensor->NuiImageStreamGetNextFrame(m_pDepthStreamHandle, 0, &imageFrame);
	
    if (FAILED(hr))
        return;
	
	BOOL nearMode;
    INuiFrameTexture* pTexture;

    // Get the depth image pixel texture
    hr = m_pNuiSensor->NuiImageFrameGetDepthImagePixelFrameTexture(
        m_pDepthStreamHandle, &imageFrame, &nearMode, &pTexture);
    if (FAILED(hr))
        goto ReleaseFrame;
	
    NUI_LOCKED_RECT LockedRect;
	
    // Lock the frame data so the Kinect knows not to modify it while we're reading it
    pTexture->LockRect(0, &LockedRect, NULL, 0);

    // Make sure we've received valid data
    if (LockedRect.Pitch != 0)
    {
        // Get the min and max reliable depth for the current frame
		int minDepth = _min_depth;//(nearMode ? NUI_IMAGE_DEPTH_MINIMUM_NEAR_MODE : NUI_IMAGE_DEPTH_MINIMUM) >> NUI_IMAGE_PLAYER_INDEX_SHIFT;
        int maxDepth = _max_depth;///(nearMode ? NUI_IMAGE_DEPTH_MAXIMUM_NEAR_MODE : NUI_IMAGE_DEPTH_MAXIMUM) >> NUI_IMAGE_PLAYER_INDEX_SHIFT;

        float * float_run = m_depth;
        const NUI_DEPTH_IMAGE_PIXEL * pBufferRun = reinterpret_cast<const NUI_DEPTH_IMAGE_PIXEL *>(LockedRect.pBits);

        // end pixel is start + width*height - 1
        const NUI_DEPTH_IMAGE_PIXEL * pBufferEnd = pBufferRun + (cDepthWidth * cDepthHeight);

		float intensity = 0.0f;
		int delta_depth =  maxDepth - minDepth;
		float float_per_depth_unit = _active_depth/float(delta_depth );
		static int t=0;
		++t;
		int aux = 0;
        while ( pBufferRun < pBufferEnd )
        {
            // discard the portion of the depth that contains only the player index
            USHORT depth = pBufferRun->depth;

			m_depthVector[aux].depth = (depth >= minDepth)? depth : 4000;
			
            // To convert to a byte, we're discarding the most-significant
            // rather than least-significant bits.
            // We're preserving detail, although the intensity will "wrap."
            // Values outside the reliable depth range are mapped to 0 (black).

            // Note: Using conditionals in this loop could degrade performance.
            // Consider using a lookup table instead when writing production code.
			//if (depth)
			intensity = static_cast<float>(depth >= minDepth && depth < maxDepth ? ((depth-minDepth)-delta_depth)*float_per_depth_unit : 0.0f);

			//if ( t < 1000)
			//	intensity = (rand()%delta_depth - delta_depth)*float_per_depth_unit;
            // Write out blue byte
            *(float_run++) = intensity;

            // Increment our index into the Kinect's depth buffer
            ++pBufferRun;
			++aux;
        }

        // Draw the data with Direct2D
        //m_pDrawDepth->Draw(m_depthRGBX, cDepthWidth * cDepthHeight * cBytesPerPixel);
		//image is ready
    }

    // We're done with the texture so unlock it
    pTexture->UnlockRect(0);

    pTexture->Release();

	ReleaseFrame:
	//return;
    // Release the frame
    m_pNuiSensor->NuiImageStreamReleaseFrame(m_pDepthStreamHandle, &imageFrame);
}

float* KinectReader::GetDepth()
{
	return m_depth;
}

vector<NUI_DEPTH_IMAGE_POINT>& KinectReader::GetDepthVector()
{
	return m_depthVector;
}

StereoKinectHeadTracking* KinectReader::GetHeadTracking()
{
	return m_headTracking;
}

/// <summary>
/// Initialize Kinect audio stream object.
/// </summary>
/// <returns>
/// <para>S_OK on success, otherwise failure code.</para>
/// </returns>
HRESULT KinectReader::InitializeAudioStream()
{
    INuiAudioBeam*      pNuiAudioSource = NULL;
    IMediaObject*       pDMO = NULL;
    IPropertyStore*     pPropertyStore = NULL;
    IStream*            pStream = NULL;

    // Get the audio source
    HRESULT hr = m_pNuiSensor->NuiGetAudioSource(&pNuiAudioSource);
    if (SUCCEEDED(hr))
    {
        hr = pNuiAudioSource->QueryInterface(IID_IMediaObject, (void**)&pDMO);
        if (SUCCEEDED(hr))
        {
            hr = pNuiAudioSource->QueryInterface(IID_IPropertyStore, (void**)&pPropertyStore);
    
            // Set AEC-MicArray DMO system mode. This must be set for the DMO to work properly.
            // Possible values are:
            //   SINGLE_CHANNEL_AEC = 0
            //   OPTIBEAM_ARRAY_ONLY = 2
            //   OPTIBEAM_ARRAY_AND_AEC = 4
            //   SINGLE_CHANNEL_NSAGC = 5
            PROPVARIANT pvSysMode;
            PropVariantInit(&pvSysMode);
            pvSysMode.vt = VT_I4;
            pvSysMode.lVal = (LONG)(2); // Use OPTIBEAM_ARRAY_ONLY setting. Set OPTIBEAM_ARRAY_AND_AEC instead if you expect to have sound playing from speakers.
            pPropertyStore->SetValue(MFPKEY_WMAAECMA_SYSTEM_MODE, pvSysMode);
            PropVariantClear(&pvSysMode);

            // Set DMO output format
            WAVEFORMATEX wfxOut = {AudioFormat, AudioChannels, AudioSamplesPerSecond, AudioAverageBytesPerSecond, AudioBlockAlign, AudioBitsPerSample, 0};
            DMO_MEDIA_TYPE mt = {0};
            MoInitMediaType(&mt, sizeof(WAVEFORMATEX));
    
            mt.majortype = MEDIATYPE_Audio;
            mt.subtype = MEDIASUBTYPE_PCM;
            mt.lSampleSize = 0;
            mt.bFixedSizeSamples = TRUE;
            mt.bTemporalCompression = FALSE;
            mt.formattype = FORMAT_WaveFormatEx;	
            memcpy(mt.pbFormat, &wfxOut, sizeof(WAVEFORMATEX));
    
            hr = pDMO->SetOutputType(0, &mt, 0);

            if (SUCCEEDED(hr))
            {
                m_pKinectAudioStream = new KinectAudioStream(pDMO);
                hr = m_pKinectAudioStream->QueryInterface(IID_IStream, (void**)&pStream);
                if (SUCCEEDED(hr))
                {
                    hr = CoCreateInstance(CLSID_SpStream, NULL, CLSCTX_INPROC_SERVER, __uuidof(ISpStream), (void**)&m_pSpeechStream);
                    if (SUCCEEDED(hr))
                        hr = m_pSpeechStream->SetBaseStream(pStream, SPDFID_WaveFormatEx, &wfxOut);
                }
            }
            MoFreeMediaType(&mt);
        }
    }

    SafeRelease(pStream);
    SafeRelease(pPropertyStore);
    SafeRelease(pDMO);
    SafeRelease(pNuiAudioSource);

    return hr;
}
/// <summary>
/// Create speech recognizer that will read Kinect audio stream data.
/// </summary>
/// <returns>
/// <para>S_OK on success, otherwise failure code.</para>
/// </returns>
HRESULT KinectReader::CreateSpeechRecognizer()
{
    ISpObjectToken *pEngineToken = NULL;
    HRESULT hr = CoCreateInstance(CLSID_SpInprocRecognizer, NULL, CLSCTX_INPROC_SERVER, __uuidof(ISpRecognizer), (void**)&m_pSpeechRecognizer);
    if (SUCCEEDED(hr))
    {
        m_pSpeechRecognizer->SetInput(m_pSpeechStream, FALSE);
        hr = SpFindBestToken(SPCAT_RECOGNIZERS,L"Language=409;Kinect=True",NULL,&pEngineToken);

        if (SUCCEEDED(hr))
        {
            m_pSpeechRecognizer->SetRecognizer(pEngineToken);
            hr = m_pSpeechRecognizer->CreateRecoContext(&m_pSpeechContext);

            // For long recognition sessions (a few hours or more), it may be beneficial to turn off adaptation of the acoustic model. 
            // This will prevent recognition accuracy from degrading over time.
            //if (SUCCEEDED(hr))
            //{
            //    hr = m_pSpeechRecognizer->SetPropertyNum(L"AdaptationOn", 0);                
            //}
        }
    }
    SafeRelease(pEngineToken);
    return hr;
}

/// <summary>
/// Load speech recognition grammar into recognizer.
/// </summary>
/// <returns>
/// <para>S_OK on success, otherwise failure code.</para>
/// </returns>
HRESULT KinectReader::LoadSpeechGrammar()
{
    HRESULT hr = m_pSpeechContext->CreateGrammar(1, &m_pSpeechGrammar);

    if (SUCCEEDED(hr))
        hr = m_pSpeechGrammar->LoadCmdFromFile(GrammarFileName, SPLO_STATIC); // Populate recognition grammar from file
    return hr;
}

/// <summary>
/// Start recognizing speech asynchronously.
/// </summary>
/// <returns>
/// <para>S_OK on success, otherwise failure code.</para>
/// </returns>
HRESULT KinectReader::StartSpeechRecognition()
{
    HRESULT hr = m_pKinectAudioStream->StartCapture();
	
    if (SUCCEEDED(hr))
    {
        // Specify that all top level rules in grammar are now active
        m_pSpeechGrammar->SetRuleState(NULL, NULL, SPRS_ACTIVE);

        // Specify that engine should always be reading audio
        m_pSpeechRecognizer->SetRecoState(SPRST_ACTIVE_ALWAYS);

        // Specify that we're only interested in receiving recognition events
        m_pSpeechContext->SetInterest(SPFEI(SPEI_RECOGNITION), SPFEI(SPEI_RECOGNITION));

        // Ensure that engine is recognizing speech and not in paused state
        hr = m_pSpeechContext->Resume(0);
        if (SUCCEEDED(hr))
            m_hSpeechEvent = m_pSpeechContext->GetNotifyEventHandle();
    }
        
    return hr;
}

/// <summary>
/// Process recently triggered speech recognition events.
/// </summary>
void KinectReader::ProcessSpeech(Input* input)
{
	m_pSpeechController = new SpeechController(input);
    const float ConfidenceThreshold = 0.6f;
	
    SPEVENT curEvent;
    ULONG fetched = 0;
    HRESULT hr = S_OK;

    m_pSpeechContext->GetEvents(1, &curEvent, &fetched);

    while (fetched > 0)
    {
        switch (curEvent.eEventId)
        {
            case SPEI_RECOGNITION:
                if (SPET_LPARAM_IS_OBJECT == curEvent.elParamType)
                {
                    // this is an ISpRecoResult
                    ISpRecoResult* result = reinterpret_cast<ISpRecoResult*>(curEvent.lParam);
                    SPPHRASE* pPhrase = NULL;
                    
                    hr = result->GetPhrase(&pPhrase);
                    if (SUCCEEDED(hr))
                    {
                        if ((pPhrase->pProperties != NULL) && (pPhrase->pProperties->pFirstChild != NULL))
                        {
                            const SPPHRASEPROPERTY* pSemanticTag = pPhrase->pProperties->pFirstChild;
                            if (pSemanticTag->SREngineConfidence > ConfidenceThreshold)
                            {
                                SpeechAction action = MapSpeechTagToAction(pSemanticTag->pszValue);
								m_pSpeechController->DoAction(action);
								
                            }
                        }
                        ::CoTaskMemFree(pPhrase);
                    }
                }
                break;
        }
        m_pSpeechContext->GetEvents(1, &curEvent, &fetched);
    }

    return;
}

/// <summary>
/// Maps a specified speech semantic tag to the corresponding action to be performed on turtle.
/// </summary>
/// <returns>
/// Action that matches <paramref name="pszSpeechTag"/>, or TurtleActionNone if no matches were found.
/// </returns>
SpeechAction KinectReader::MapSpeechTagToAction(LPCWSTR pszSpeechTag)
{
    struct SpeechTagToAction
    {
        LPCWSTR pszSpeechTag;
        SpeechAction action;
    };
    const SpeechTagToAction Map[] =
    {
		/// Stage 1 /// 
        {L"MODEL", SpeechActionModel},				// model
        {L"CAMERA", SpeechActionCamera},			// camera
        {L"SOUND", SpeechActionSound},				// sound
        {L"HAPTICS", SpeechActionHaptics},			// haptics

		///  Stage 2 || 3 || 4 || 5 || 6 || 7 ///
        {L"BACK", SpeechActionBack},				// back	
		
		/// Stage 2 /// 
        {L"REINITIATE", SpeechActionReinitiate},	// reinititate	
        {L"SPIN", SpeechActionSpin},				// spin	
        {L"ROTATE", SpeechActionRotate},			// rotate
		
		/// Stage 3 || 4 || 5 /// 
        {L"RIGHT", SpeechActionRight},				// right	
        {L"LEFT", SpeechActionLeft},				// left	

		/// Stage 3 ///
        {L"UP", SpeechActionUp},					// up	
        {L"DOWN", SpeechActionDown},				// down	
        {L"CLOCKWISE", SpeechActionClockwise},		// clockwise	
        {L"COUNTER_CLOCKWISE", SpeechActionCounterClockwise}, // counter clockwise
        {L"FREEZ", SpeechActionFreezRotation},		// freez

        {L"FASTER", SpeechActionFaster},			// faster	
        {L"SLOWER", SpeechActionSlower},			// slower	

		///  Stage 3 || 4  ///
        {L"CLEAR", SpeechActionClearRotation},		// clear		
		
		/// Stage 4 || 5  /// 
        {L"FRONT", SpeechActionFront},				// front	
        {L"TOP", SpeechActionTop},					// top	
        {L"BOTTOM", SpeechActionBottom},			// bottom	
        {L"REAR", SpeechActionRear},				// rear	
		
		
		/// Stage 6 /// 
        {L"ON", SpeechActionON},					// ON
        {L"OFF", SpeechActionOFF},					// OFF	
				
		/// Stage Always on /// 
        {L"START", SpeechActionBegin},				// start
        {L"END", SpeechActionEnd},					// top	
        {L"QUIT", SpeechActionQuit}					// quit program	
	};
    SpeechAction action = SpeechActionNone;

    for (int i = 0; i < _countof(Map); ++i)
    {
        if (0 == wcscmp(Map[i].pszSpeechTag, pszSpeechTag))
        {
            action = Map[i].action;
            break;
        }
    }

    return action;
}

/// <summary>
/// Handle new skeleton data
/// </summary>
void KinectReader::ProcessSkeleton()
{
    NUI_SKELETON_FRAME skeletonFrame = {0};
    HRESULT hr = m_pNuiSensor->NuiSkeletonGetNextFrame(0, &skeletonFrame);
    if ( FAILED(hr))
        return;
    // smooth out the skeleton data
    m_pNuiSensor->NuiTransformSmooth(&skeletonFrame, NULL);
}
	
	
/*	
void KinectReader::SkeletonFrameReady(NUI_SKELETON_FRAME* pSkeletonFrame)
{
    for (int i = 0; i < NUI_SKELETON_COUNT; i++)
    {
        const NUI_SKELETON_DATA & skeleton = pSkeletonFrame->SkeletonData[i];
		/*
        switch (skeleton.eTrackingState)
        {
			case NUI_SKELETON_TRACKED:
				//DrawTrackedSkeletonJoints(skeleton);
				break;
 
			case NUI_SKELETON_POSITION_ONLY:
				//DrawSkeletonPosition(skeleton.Position);
				break;
        }
    }
}
void KinectReader::DrawSkeleton(const NUI_SKELETON_DATA & skeleton)
{
	DrawBone(skeleton, NUI_SKELETON_POSITION_WRIST_RIGHT, NUI_SKELETON_POSITION_HAND_RIGHT);
}
void KinectReader::DrawBone(const NUI_SKELETON_DATA & skeleton,
      NUI_SKELETON_POSITION_INDEX jointFrom,
      NUI_SKELETON_POSITION_INDEX jointTo)
{

}*/