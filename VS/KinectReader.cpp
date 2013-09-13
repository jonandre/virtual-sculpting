#include "KinectReader.h"
#include "main.h"
#include "NuiApi.h"

KinectReader::KinectReader(void)
{
	
}

KinectReader::KinectReader( unsigned int min_depth, unsigned int max_depth, float dist )
{
	m_depth = new float[640*480];
	memset(m_depth, 0, 640*480*sizeof(float));
	m_pNuiSensor = NULL;
	CreateFirstConnected();
	_min_depth = min_depth;
	_max_depth = max_depth;
	_active_depth = dist;
}


KinectReader::~KinectReader(void)
{
	delete [] m_depth;
}

HRESULT KinectReader::CreateFirstConnected()
{
    INuiSensor * pNuiSensor = NULL;
    HRESULT hr;
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
        {
            continue;
        }

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
        // Initialize the Kinect and specify that we'll be using depth
        hr = m_pNuiSensor->NuiInitialize(NUI_INITIALIZE_FLAG_USES_DEPTH); 
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
        }
    }

    if (NULL == m_pNuiSensor || FAILED(hr))
    {
		std::cout<<"No ready Kinect found!"<<std::endl;
        //SetStatusMessage(L"No ready Kinect found!");
        return E_FAIL;
    }

    return hr;
}

void KinectReader::ProcessDepth()
{
	if(!m_pNuiSensor)
	{/*
		int delta_depth =  _max_depth - _min_depth;
		for ( unsigned int i = 0; i < 640*160; i++ )
			m_depth[i] = (rand()%delta_depth - delta_depth)*0.15f;
			*/

		return;
	}


    HRESULT hr;
    NUI_IMAGE_FRAME imageFrame;

    // Attempt to get the depth frame
    hr = m_pNuiSensor->NuiImageStreamGetNextFrame(m_pDepthStreamHandle, 0, &imageFrame);
	
    if (FAILED(hr))
    {
        return;
    }

    BOOL nearMode;
    INuiFrameTexture* pTexture;

    // Get the depth image pixel texture
    hr = m_pNuiSensor->NuiImageFrameGetDepthImagePixelFrameTexture(
        m_pDepthStreamHandle, &imageFrame, &nearMode, &pTexture);
    if (FAILED(hr))
    {
        goto ReleaseFrame;
    }
	
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
        while ( pBufferRun < pBufferEnd )
        {
            // discard the portion of the depth that contains only the player index
            USHORT depth = pBufferRun->depth;

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

