#define SDLCONTEXT

#include "GridModel.h"
#ifdef SDLCONTEXT
#include "SDLContext.h"
#else
#include "GLContext.h"
#endif

#include <time.h>
#include "KinectTool.h"
#include "StereoKinectHeadTracking.h"
#include "KinectReader.h"
#include "TriangleMesh.h"
#include "Soundify.h"
#include "texturemappedfont.h"
#include "Stage.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <Winsock.h>
#include <Windows.h>
#include <iostream>
#include <MMSystem.h>

#pragma comment(lib, "ws2_32.lib")


extern float fps_procentage;


void SoundAndHaptics(void)
{
	if (space_pressed)
	{
		
		if (acted <= 1)
			waveOutSetVolume(NULL, 1);
		else if (acted >= 180000)
		{
			if (GetSoundStage())
			{
				waveOutSetVolume(NULL, 180000);
			}
		}
		else
		{
			if (GetSoundStage())
			{
				waveOutSetVolume(NULL, acted*1000);
			}
		}

		if (!GetSoundStage())
			RemoveSound();
		if (hapticsConnected)
		{
			if (!GetHapticsStage())
			{
				RemoveHaptics();
			}
		}
		if (acted)
		{
			if (!hand_is_on)
			{
				if (GetHapticsStage() && hapticsConnected)
					system("C:/Python33/python.exe gloves.py on");
				else
				{
					if (hapticsConnected)
						RemoveHaptics();
				}
				if (!GetSoundStage())
					RemoveSound();

				hand_is_on = true;
			}
		}
		else
		{
			if (hand_is_on)
			{
				if (GetHapticsStage() && hapticsConnected)
					system("C:/Python33/python.exe gloves.py off");
				else
				{
					if (hapticsConnected)
						RemoveHaptics();
				}
				if (GetSoundStage())
					waveOutSetVolume(NULL, 100);
				else
					RemoveSound();

				hand_is_on = false;
			}
		}
		
	}
	else
	{
		RemoveSound();

		if (hapticsConnected)
			RemoveHaptics();
	}
}

inline double diffclock( clock_t clock1, clock_t clock2 ) 
{
	double diffticks = clock1 - clock2;
    double diffms    = diffticks / ( CLOCKS_PER_SEC / 1000 );

    return diffms;
}

int main( int argc, char** argv) 
{
	/* Intilising the stage */
	printf("Initilising stage... \n");
	new Stage();

	/* Initates the screen and its context */
	#ifdef SDLCONTEXT
	SDLContext* cntx = new SDLContext();//Window+render.
	#else
	GLContext* cntx = new GLContext();//Window+render.
	#endif
	
	/* Initilaizes the input */
	Input* inp = new Input();//Input system.
	cntx->SetInput( inp );//Context redirects mouse+keyb to Input

	/* Initializes the model */
	unsigned int power = 8;
	GridModel* model = new GridModel(power);//power of 2
	unsigned int side = model->GetDimm();
	std::cout << "Model side is " << side << std::endl;
	inp->SetZoom(-(side*4.0f));
	inp->SetModel( model );
	std::cout << "Model initialized" << std::endl;
	model->UpdateGrid();// update visual representation of model
	std::cout << "Grid updated" << std::endl;

	/* Initilizes the tool */
	KinectTool* tool = new KinectTool( (side*0.75f), (side*0.75f), side*.75f+ 100, -(side*.75f));

	/* Initializes head tracking */
	StereoKinectHeadTracking* headTracking = new StereoKinectHeadTracking();
	tool->_reader->Init(headTracking);
	
	/* Initilizes the the font and text */
	int fontSize = 20;
	int dif = 5;
	TextureMappedFont* font1 = new TextureMappedFont("font1.bmp", fontSize);
	TextureMappedFont* font2 = new TextureMappedFont("font1.bmp", fontSize + dif);
	TextureMappedFont* font3 = new TextureMappedFont("font1.bmp", fontSize + 2*dif);

	/* Initilizes Windows socket */
	//WSADATA wsaData;
	//WSAStartup(0x0202, &wsaData);


	void** speech_thread_args = (void**)malloc(2 * sizeof(void*));
	speech_thread_args[0] = (void*)tool;
	speech_thread_args[1] = (void*)inp;
	pthread_t speechThread;
	pthread_create(&speechThread, NULL, SpeechThreed, speech_thread_args);
	
    
	PlaySound(TEXT("VS-midle-sound.wav"), NULL, SND_LOOP | SND_ASYNC);
	
	std::cout << "Starting main loop" << std::endl;

	clock_t start = clock();
	while (cntx->alive())
	{
		inp->UpdateFrame();		//Reset frame variables.
		cntx->doMessage();		//Win message loop

		tool->DoToolUpdate();	//update tool state - like depthmap
		headTracking->Update();

		//
		headTracking->GetHeadPosition();
		//

		if ( GetPressedStage() )
			tool->StartInteractModel( model, inp->GetObjectQ());//obvious

		cntx->renderScene(model, tool, inp->GetViewM(), inp->GetObjectM(), font1, font2, font3);// do actual rendering.
		
		if ( GetPressedStage() )
			acted = tool->StopInteractModel( );//obvious

		model->UpdateGrid();			// update visual representation of model
		
		SoundAndHaptics();
		
		clock_t end = clock();
		float fps = CLOCKS_PER_SEC / (float)(end - start);
		start = end;
		fps_procentage = 30.f / fps;
	}
	
	RemoveSound();
	if (hapticsConnected)
		RemoveHaptics();

	delete model;
	delete inp;
	delete tool;
	delete cntx;
	return 0;
}

