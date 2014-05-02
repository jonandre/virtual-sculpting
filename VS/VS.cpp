#include "main.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <iostream>
#include <MMSystem.h>

#include "GridModel.h"
#include "SDLContext.h"

#include <time.h>
#include "KinectTool.h"
#include "StereoKinectHeadTracking.h"
#include "KinectReader.h"
#include "TriangleMesh.h"
#include "Soundify.h"
#include "texturemappedfont.h"
#include "Stage.h"

#pragma comment(lib, "ws2_32.lib")


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

int main( int argc, char** argv) 
{
	/* Intilising the stage */
	printf("Initilising stage... \n");
	new Stage();

	/* Initates the screen and its context */
	SDLContext* cntx = new SDLContext();//Window+render
	
	/* Initilaizes the input */
	Input* inp = new Input();//Input system.
	cntx->SetInput( inp );//Context redirects mouse+keyb to Input

	/* Initializes the model */
	unsigned int power = 7;
	GridModel* model = new GridModel(power);//power of 2
	//inp->SetZoom(-(side*4.0f));
	inp->SetModel( model );
	inp->SetModelSide(0.5f);
	inp->SetModelPosition(glm::vec3(0.0f, 0.0f, 1.25f));
	float side = inp->GetModelSide();
	std::cout << "Model side is " << side << std::endl;
	std::cout << "Model initialized" << std::endl;
	model->UpdateGrid();// update visual representation of model
	std::cout << "Grid updated" << std::endl;

	/* Initilizes the tool */
	KinectTool* tool = new KinectTool( (side*0.75f), (side*0.75f), side*0.75f + 100, -(side*.75f));

	/* Initializes head tracking */
	StereoKinectHeadTracking* headTracking = new StereoKinectHeadTracking();
	tool->_reader->Init(headTracking);

	cntx->SetHeadTracking(headTracking);
	
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

	clock_t start = clock() - 1;
	while (cntx->alive())
	{
		clock_t end = clock();
		float deltaTime = (float)(end - start) / CLOCKS_PER_SEC;

		inp->UpdateFrame(deltaTime);		//Reset frame variables.
		cntx->doMessage();		//Win message loop

		tool->DoToolUpdate();	//update tool state - like depthmap
		headTracking->Update(deltaTime);

		//
		headTracking->GetHeadPosition();
		//

		if ( GetPressedStage() )
			tool->StartInteractModel( model, inp->GetObjectQ(), inp->GetObjectM());//obvious

		cntx->renderScene(model, tool, inp->GetViewM(), inp->GetObjectM(), font1, font2, font3);// do actual rendering.
		
		if ( GetPressedStage() )
			acted = tool->StopInteractModel( );//obvious

		model->UpdateGrid();			// update visual representation of model
		
		SoundAndHaptics();

		start = end;
	}
	
	RemoveSound();
	if (hapticsConnected)
		RemoveHaptics();

	delete model;
	delete inp;
	delete tool;
	delete cntx;
	delete headTracking;
	return 0;
}

