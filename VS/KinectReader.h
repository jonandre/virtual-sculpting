#ifndef KINECT_READER_H
#define KINECT_READER_H

#include "Windows.h"
#include "NuiApi.h"

/* Voice Recognition Start */
#include "SpeechController.h"
// For configuring DMO properties
#include <wmcodecdsp.h>
// For FORMAT_WaveFormatEx and such
#include <uuids.h>
#include "KinectAudioStream.h"
#include "resource.h"
// For speech APIs
// NOTE: To ensure that application compiles and links against correct SAPI versions (from Microsoft Speech
//       SDK), VC++ include and library paths should be configured to list appropriate paths within Microsoft
//       Speech SDK installation directory before listing the default system include and library directories,
//       which might contain a version of SAPI that is not appropriate for use together with Kinect sensor.
#include <sapi.h>
#include <sphelper.h>
/* Voice Recognition End */

#include "StereoKinectHeadTracking.h"

#include <vector>

/// <summary>
/// Application class for using kinect.
/// </summary>
class KinectReader
{
public:
    /// Constructor
	KinectReader( unsigned int min_depth, unsigned int max_depth, float dist);
    /// Destructor
	~KinectReader();

	void Init(StereoKinectHeadTracking* headTracking);
	
	StereoKinectHeadTracking* GetHeadTracking();

	float* GetDepth();
	vector<NUI_DEPTH_IMAGE_POINT>& GetDepthVector();
	void ProcessDepth();

	/* Voice Recognition Start */
	static LPCWSTR          GrammarFileName;
	// Object that controls moving changes in seen due to voice recognition
    SpeechController*		m_pSpeechController;
	// Audio stream captured from Kinect.
    KinectAudioStream*      m_pKinectAudioStream;
	// Stream given to speech recognition engine
    ISpStream*              m_pSpeechStream;
	// Speech recognizer
    ISpRecognizer*          m_pSpeechRecognizer;
    // Speech recognizer context
    ISpRecoContext*         m_pSpeechContext;
    // Speech grammar
    ISpRecoGrammar*         m_pSpeechGrammar;
    // Event triggered when we detect speech recognition
    HANDLE                  m_hSpeechEvent;
    /// Initialize Kinect audio stream object.
    HRESULT                 InitializeAudioStream();
    /// Create speech recognizer that will read Kinect audio stream data.
    HRESULT                 CreateSpeechRecognizer();
    /// Load speech recognition grammar into recognizer.
    HRESULT                 LoadSpeechGrammar();
    /// Start recognizing speech asynchronously.
    HRESULT                 StartSpeechRecognition();
    /// Process recently triggered speech recognition events.
    void                    ProcessSpeech(Input*);
    /// Maps a specified speech semantic tag to the corresponding action to be performed on turtle.
    SpeechAction            MapSpeechTagToAction(LPCWSTR pszSpeechTag);
	/* Voice Recognition End */

	/* Skelleton Recognition Start */
    /// Handle new skeleton data when uppdating
    void                    ProcessSkeleton();
	HANDLE                  m_hNextSkeletonEvent;	
	/* Skelleton Recognition End */

private:
	KinectReader();
	unsigned int _min_depth;
	unsigned int _max_depth;
	float        _active_depth;
	
	StereoKinectHeadTracking* m_headTracking;

	// Current Kinect
    INuiSensor*      m_pNuiSensor;
	static const int cDepthWidth  = 640;
    static const int cDepthHeight = 480;
    static const int cBytesPerPixel = 1;

	float*					m_depth;
	vector<NUI_DEPTH_IMAGE_POINT> m_depthVector;
	
    /// Create the first connected Kinect found.
	HRESULT                 CreateFirstConnected(StereoKinectHeadTracking* headTracking);

	/* Depth */ 
	HANDLE                  m_hNextDepthFrameEvent;
	HANDLE                  m_pDepthStreamHandle;
	/* Skelleton */ 
	HANDLE                  m_pSkeletonStreamHandle;
	//void SkeletonFrameReady(NUI_SKELETON_FRAME*);
	//void DrawSkeleton(NUI_SKELETON_DATA*);
};

#endif // KINECT_READER_H