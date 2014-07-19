#include "SDLContext.h"
#include "GridModel.h"

const float DEG_TO_RAD = ((2.0*M_PI) / 360.0);

void inline checkSDLError(int line = -1) {
	const char* error = SDL_GetError();
	if (*error != '\0') {
		std::cout << "SDL Error: " << error;
		if (line != -1) std::cout << " in line: " << line;
		std::cout << std::endl;
		SDL_ClearError();
	}
}

SDLContext::SDLContext()
{
	SDL_Init(SDL_INIT_EVERYTHING);
	checkSDLError(__LINE__);

	int numDisplays = SDL_GetNumVideoDisplays();
	std::cout << "Number of displays detected: " << numDisplays << std::endl;

	if (numDisplays > 4) {
		std::cout << "Enabling shadow rendering on display 1" << std::endl;
		shadowRendering = true;
	}

	// Main Window
#ifdef VIC4K
	SCREEN_WIDTH = 4096;
	SCREEN_HEIGHT = 2400;
#else
	// Comment screen width/height settings to get fullscreen
	SCREEN_WIDTH = 1920;
	SCREEN_HEIGHT = 1080;
#endif
	window = SDL_CreateWindow("Virtual Sculpting", 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_OPENGL | SDL_WINDOW_BORDERLESS
#ifndef VIC4K
#if (STEREO == 0)
		| SDL_WINDOW_FULLSCREEN_DESKTOP
#endif
#endif
		);
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	int w, h;
	SDL_GetWindowSize(window, &w, &h);
	std::cout << "Main Window created, size: " << w << " x " << h << std::endl;
	checkSDLError(__LINE__);
	
	SDL_GL_SetAttribute(SDL_GL_SHARE_WITH_CURRENT_CONTEXT, 0);
	context = SDL_GL_CreateContext(window);
	checkSDLError(__LINE__);

	GLenum error = glewInit();
	if (error != GLEW_OK) {
		std::cout << "FATAL: Could not init GLEW" << std::endl;
		std::cout << "Error: " << glewGetErrorString(error) << std::endl;
	}

#if (STEREO > 0)
	{
		SDL_GL_SetAttribute(SDL_GL_SHARE_WITH_CURRENT_CONTEXT, 1);
		windowRight = SDL_CreateWindow("Virtual Sculpting 2", SCREEN_WIDTH, 0, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_OPENGL | SDL_WINDOW_BORDERLESS);
		rendererRight = SDL_CreateRenderer(windowRight, -1, SDL_RENDERER_ACCELERATED);
		SDL_GetWindowSize(windowRight, &w, &h);
		std::cout << "Right Window created, size: " << w << " x " << h << std::endl;
		checkSDLError(__LINE__);
		contextRight = SDL_GL_CreateContext(windowRight);
		checkSDLError(__LINE__);

		if (shadowRendering) { // Shadow window
			SDL_GL_SetAttribute(SDL_GL_SHARE_WITH_CURRENT_CONTEXT, 1);
			windowShadow = SDL_CreateWindow("Virtual Sculpting 3", SCREEN_WIDTH*2, 0, 800, 600, SDL_WINDOW_OPENGL | SDL_WINDOW_BORDERLESS);
			rendererShadow = SDL_CreateRenderer(windowShadow, -1, SDL_RENDERER_ACCELERATED);
			SDL_GetWindowSize(windowShadow, &w, &h);
			std::cout << "Shadow Window created, size: " << w << " x " << h << std::endl;
			checkSDLError(__LINE__);
			contextShadow = SDL_GL_CreateContext(windowShadow);
			checkSDLError(__LINE__);

			// Post-context creation flags
			//SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
			//SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 4);
			SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
			SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
			SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
			SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
			SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
			SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
			SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 32);
			checkSDLError(__LINE__);

			GLenum error = glewInit();
			if (error != GLEW_OK) {
				std::cout << "FATAL: Could not init GLEW" << std::endl;
				std::cout << "Error: " << glewGetErrorString(error) << std::endl;
			}

			wglShareLists((HGLRC)context, (HGLRC)contextShadow);

		}

		// Post-context creation flags
		//SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
		//SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 4);
		SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
		SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
		SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
		SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
		SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
		SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
		SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 32);
		checkSDLError(__LINE__);

		GLenum error = glewInit();
		if (error != GLEW_OK) {
			std::cout << "FATAL: Could not init GLEW" << std::endl;
			std::cout << "Error: " << glewGetErrorString(error) << std::endl;
		}

		wglShareLists((HGLRC)context, (HGLRC)contextRight);
	}
#endif

	SDL_GL_MakeCurrent(window, context);

	
	// Post-context creation flags
	//SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	//SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 4);
	SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 32);
	checkSDLError(__LINE__);

	running = true;
	render = NULL;

	int glVersion[2] = {-1, -1}; 
	glGetIntegerv(GL_MAJOR_VERSION, &glVersion[0]);
	glGetIntegerv(GL_MINOR_VERSION, &glVersion[1]);

	std::cout << "Using OpenGL: " << glVersion[0] << "." << glVersion[1] << std::endl; // Output which version of OpenGL we are using

	// Scene rendering
#if (STEREO > 0)
	render = new StereoRender();
	render->Init();
	render->Resize( SCREEN_WIDTH, SCREEN_HEIGHT );
#else
	render = new Render();
	render->Init();
	render->Resize(SCREEN_WIDTH, SCREEN_HEIGHT);
#endif

	// FPS count
	FPS = 1.0f;
	lastTick = SDL_GetTicks();
	lastFPSTick = lastTick;

	speed = 1.0f;
	depth = 0.0f; 
	wantedDepth = 0.0f;

	// SCENE VIEW
	showScene= false;
	sceneRotSpeed = 5.0f;
	SCENE_PREVIW_SIZE = SCREEN_HEIGHT/4;

	sceneProj = glm::perspective(45.0f, 1.0f, 0.1f, 100.0f);

	// PROJECTOR SHADOW 0.92, 1.42
	float projW = 1.39f;
	float projH = 0.925f;
	float projDepth = 5.0f;
	//Relative to the center of the screen, in meters
	float projPosX = 0.1225f;
	float projPosY = 3.0f;
	float projPosZ =  0.855 + projW/2.0f;

	shadowProj = glm::ortho(-projW/2.0f, projW/2.0f, -projH/2.0f, projH/2.0f, 0.1f, projDepth);
	shadowView = glm::lookAt(glm::vec3(projPosX, projPosY,projPosZ), glm::vec3(projPosX, projPosY-projDepth, projPosZ), glm::vec3(1.0f, 0.0f, 0.0f));


	std::cout << "SDLContext initialized" << std::endl;
}

void SDLContext::SetInput( Input* input)
{
	inp = input;
}

SDLContext::~SDLContext() 
{
	SDL_GL_DeleteContext(context);
#if (STEREO > 0)
	SDL_GL_DeleteContext(contextRight);
#endif
	if (shadowRendering)
		SDL_GL_DeleteContext(contextShadow);
	SDL_Quit();
	delete render;
}


void SDLContext::doMessage()
{
	SDL_Event e;
	while (SDL_PollEvent(&e) != 0) 
	{
		if (e.type == SDL_QUIT || (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_ESCAPE)) 
			{
				running = false;
			}
		else 
		{
			if ( e.type == SDL_KEYDOWN )
			{			
				inp->OnKeyPressed(e.key.keysym.sym);

				if (e.key.keysym.sym == SDLK_PERIOD) {
					showScene ^= true;
				}
			}
			else if ( e.type == SDL_KEYUP )
			{
				inp->OnKeyReleased(e.key.keysym.sym);
			}

			// TODO: These need to be changed to SDL2 events

			/*else if ( msg.message == WM_LBUTTONDOWN )
			{
				inp->OnMouseLBDown(msg.pt.x, msg.pt.y);
			}
			else if ( msg.message == WM_LBUTTONUP)
			{
				inp->OnMouseLBUp(msg.pt.x, msg.pt.y);
			}
			else if ( msg.message == WM_MOUSEMOVE )
			{
				inp->OnMouseMove(msg.pt.x, msg.pt.y);
			}
			else if ( msg.message == WM_MOUSEWHEEL )
			{
				inp->OnSroll( GET_WHEEL_DELTA_WPARAM(msg.wParam) );
			}*/
		}
	}
}


void SDLContext::SetHeadTracking(StereoKinectHeadTracking* headTracking) {
	this->headTracking = headTracking;

	// All in Meters
	float VIC_SCREEN_WIDTH = 4.0055f;
	float VIC_SCREEN_HEIGHT = 2.430f;
	float VIC_SCREEN_BORDER = 0.075f;
	float EYE_DISTANCE = 0.065f;
	float HEAD_DEPTH_RADIUS = 0.1f;

	headTracking->SetDisplaySize(VIC_SCREEN_WIDTH - VIC_SCREEN_BORDER*2.0f,  VIC_SCREEN_HEIGHT - VIC_SCREEN_BORDER*2.0f);
	headTracking->SetEyeDistance(EYE_DISTANCE);
	headTracking->SetHeadRadius(HEAD_DEPTH_RADIUS);

	// Relative to the center of the screen
	headTracking->SetSensorPosition(0.13f, ((VIC_SCREEN_HEIGHT - VIC_SCREEN_BORDER*2.0f)/2.0f + 0.0375f), 0.13f);
	headTracking->SetInterestFacing(true);
	
	headTracking->SetViewportSize(VIC_SCREEN_WIDTH - VIC_SCREEN_BORDER*2.0f, VIC_SCREEN_HEIGHT - VIC_SCREEN_BORDER*2.0f);
	headTracking->SetZPlanes(render->ZNEAR, render->ZFAR);	
}


bool SDLContext::alive()
{
	return running;
}

void SDLContext::renderScene( GridModel* model, KinectTool* _tool_mesh, 
							glm::mat4& view, glm::mat4& obj, TextureMappedFont* font1, 
							TextureMappedFont* font2, TextureMappedFont* font3)
{
	

#if (STEREO > 0)
	float side = inp->GetModelSide();

	float ratio  = float(SCREEN_WIDTH) / float(SCREEN_HEIGHT);

	sceneView = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -0.5f, -4.0f));
	sceneView = glm::rotate(sceneView, sceneRot, glm::vec3(0.0f, 1.0f, 0.0f));
	sceneView = glm::translate(sceneView, -headTracking->GetHeadPosition());

	//sceneView = glm::lookAt(headTracking->GetHeadPosition() + glm::vec3(0.0f, 0.2f, 4.0f), headTracking->GetHeadPosition(), glm::vec3(0.0f, 1.0f, 0.0f)); 
	//sceneView = glm::rotate(sceneView, sceneRot, glm::vec3(0.0f, 1.0f, 0.0f));

	headTracking->RetrieveMatrices(inp->GetObjectPosition(), leftProj, leftEye, rightProj, rightEye);

	glm::vec3 cameraPosition (0.0f, 0.0f, depth*side);
	
	glm::mat4 cameraView = glm::translate(glm::mat4(1.0f), cameraPosition);

	render->Draw(model, _tool_mesh, leftEye*cameraView, obj, font1, font2, font3, leftProj, 0);
	
	if (showScene) {
		glm::mat4 leftSceneView = glm::translate(glm::mat4(1.0f), glm::vec3(-0.065f/2.0f, 0.0f, 0.0f));

		glViewport(SCREEN_WIDTH/2 - SCENE_PREVIW_SIZE/2,SCREEN_HEIGHT - SCENE_PREVIW_SIZE,SCENE_PREVIW_SIZE,SCENE_PREVIW_SIZE);
		
		glEnable(GL_SCISSOR_TEST); 
		glScissor(SCREEN_WIDTH/2 - SCENE_PREVIW_SIZE/2,SCREEN_HEIGHT - SCENE_PREVIW_SIZE,SCENE_PREVIW_SIZE,SCENE_PREVIW_SIZE);

		render->Draw(model, _tool_mesh, leftSceneView*sceneView, obj, font1, font2, font3, sceneProj, 3);

		glDisable(GL_SCISSOR_TEST); 

		glViewport(0,0,SCREEN_WIDTH,SCREEN_HEIGHT);
	}

	SDL_GL_SwapWindow(window);

	SDL_GL_MakeCurrent(windowRight, context);

	render->Draw( model, _tool_mesh, rightEye*cameraView, obj, font1, font2 , font3, rightProj, 1);

	if (showScene) {
		glm::mat4 rightSceneView = glm::translate(glm::mat4(1.0f), glm::vec3(0.065f/2.0f, 0.0f, 0.0f));

		glViewport(SCREEN_WIDTH/2 - SCENE_PREVIW_SIZE/2,SCREEN_HEIGHT - SCENE_PREVIW_SIZE,SCENE_PREVIW_SIZE,SCENE_PREVIW_SIZE);

		glEnable(GL_SCISSOR_TEST); 
		glScissor(SCREEN_WIDTH/2 - SCENE_PREVIW_SIZE/2,SCREEN_HEIGHT - SCENE_PREVIW_SIZE,SCENE_PREVIW_SIZE,SCENE_PREVIW_SIZE);

		render->Draw(model, _tool_mesh, rightSceneView*sceneView, obj, font1, font2, font3, sceneProj, 3);
		
		glDisable(GL_SCISSOR_TEST); 

		glViewport(0,0,SCREEN_WIDTH,SCREEN_HEIGHT);
	}

	SDL_GL_SwapWindow(windowRight);
#else
	render->Draw(model, _tool_mesh, view, obj, font1, font2, font3);

	SDL_GL_SwapWindow(window);
#endif

	if (shadowRendering) {
		SDL_GL_MakeCurrent(windowShadow, context);

		glViewport(0,0,800,600);

		render->Draw( model, _tool_mesh, shadowView, obj, font1, font2 , font3, shadowProj, -1);

		SDL_GL_SwapWindow(windowShadow);

		glViewport(0,0,SCREEN_WIDTH,SCREEN_HEIGHT);
	}
	
	SDL_GL_MakeCurrent(window, context);

	// FPS Control
	int tick = SDL_GetTicks();
	int deltaTick = tick - lastTick;
	float frameTime = ((float) deltaTick)/1000.0f;
	FPS = FPS*0.9f + (1.0f/frameTime)*0.1f;
	lastTick = tick;

	if (tick - lastFPSTick > FPSPeriod) {
		std::cout << "FPS: " << FPS << std::endl;
		lastFPSTick = tick;
	}

	sceneRot += sceneRotSpeed*frameTime;

	depth = depth*(speed - frameTime)/speed + wantedDepth*frameTime/speed;
}

