#include "GridModel.h"
#include "GLContext.h"
#include <time.h>
#include "KinectTool.h"
#include "TriangleMesh.h"
#include "Soundify.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <Winsock.h>
#include <Windows.h>
#include <iostream>
#include <MMSystem.h>

#pragma comment(lib, "ws2_32.lib")

void hand_on(bool a, bool b);
void hand_off(bool a, bool b);

inline double diffclock( clock_t clock1, clock_t clock2 ) 
{
	double diffticks = clock1 - clock2;
    double diffms    = diffticks / ( CLOCKS_PER_SEC / 1000 );

    return diffms;
}

int main( int argc, UINT8** argv) 
{
	GLContext* cntx = new GLContext();//Window+render.
	Input* inp = new Input();//Input system.
	cntx->SetInput( inp );//Context redirects mouse+keyb to Input
	

	unsigned int power = 8;

	GridModel* model = new GridModel(power);//power of 2
	unsigned int side = model->GetDimm();
	inp->SetZoom(-(side*4.0f));
	inp->SetModel( model );

	model->UpdateGrid();// update visual representation of model
	KinectTool* tool = new KinectTool( (side*0.75f), (side*0.75f), side*.75f, -(side*.75f));

	Soundify snd;
	snd.Play();

	int acted = 0;

	/* Windows socket startup */
	WSADATA wsaData;
	WSAStartup(0x0202, &wsaData);


	void** speech_thread_args = (void**)malloc(2 * sizeof(void*));
	speech_thread_args[0] = (void*)tool;
	speech_thread_args[1] = (void*)inp;
	pthread_t speechThread;
	pthread_create(&speechThread, NULL, SpeechThreed, speech_thread_args);
	
    

	while (cntx->alive())
	{	
		clock_t start = clock();
		acted = 0;

		inp->UpdateFrame();		//Reset frame variables.
		cntx->doMessage();		//Win message loop


		tool->DoToolUpdate();	//update tool state - like depthmap
		
		if ( inp->IsPressed(' ') )
		{
			inp->space_pressed = !(inp->space_pressed);
		}

		if ( inp->space_pressed )
			tool->StartInteractModel( model, inp->GetObjectQ());//obvious
		
		
		cntx->renderScene(model, tool, inp->GetViewM(), inp->GetObjectM());// do actual rendering.

		if ( inp->space_pressed )
			acted = tool->StopInteractModel( );//obvious

		model->UpdateGrid();			// update visual representation of model
		
		///
		//static int tmp = 0;
		//tmp++;
		//acted = (rand()%100) > 10 ? (rand()%3000) : 0;
		//acted = tmp;
		///

		if (acted)
			hand_on(inp->useHaptics, inp->useSound);
		else
			hand_off(inp->useHaptics, inp->useSound);
			
	}

	snd.SetGain(0.0f);

	delete model;
	delete inp;
	delete tool;
	delete cntx;
	return 0;
}

int hand_is_on = 1;


void hand_on(bool a, bool b)
{
	if (!hand_is_on)
	{
		if (a == true)
		{
			system("C:/Python33/python.exe gloves.py on");
		}
		
		if (b == true){
			//PlaySound(TEXT("VS-start-sound.wav"), NULL, SND_ASYNC); 
			PlaySound(TEXT("VS-midle-sound.wav"), NULL, SND_LOOP | SND_ASYNC);
		}
		/*
		std::cerr << "Send on" << std::endl;
		sendto(sock, hand_on_packet, 16, 0, (struct sockaddr*)&send_address, sizeof(send_address));
		std::cerr << "Send on again" << std::endl;
		sendto(sock, hand_on_packet, 16, 0, (struct sockaddr*)&send_address, sizeof(send_address));
		std::cerr << "Send on done" << std::endl << std::endl;
		*/
		hand_is_on = 1;
	} 
	//else if (hand_is_on == 1)
		
}

/**
 * Send signal to the Arduino to turn off feedback actions
 */
void hand_off(bool a, bool b)
{
	if (hand_is_on)
	{
		if (a == true)
		{
			system("C:/Python33/python.exe gloves.py off");
		}
		if (b == true){
			PlaySound(NULL, 0, 0);
			PlaySound(TEXT("VS-end-sound.wav"), NULL, SND_ASYNC); 
		}
		/*
			std::cerr << "Send off" << std::endl;
			sendto(sock, hand_off_packet, 16, 0, (struct sockaddr*)&send_address, sizeof(send_address));
			std::cerr << "Send off again" << std::endl;
			sendto(sock, hand_off_packet, 16, 0, (struct sockaddr*)&send_address, sizeof(send_address));
			std::cerr << "Send off done" << std::endl << std::endl;
		*/
		hand_is_on = 0;
	}
}