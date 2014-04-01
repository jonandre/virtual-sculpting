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

	std::cout << "Number of displays detected: " << SDL_GetNumVideoDisplays() << std::endl;

	int hackPixels = 0;

	// Main Window
#ifdef VIC4K
	SCREEN_WIDTH = 4096;
	SCREEN_HEIGHT = 2400;
#else
	// Comment screen width/height settings to get fullscreen
	SCREEN_WIDTH = 1920;
	SCREEN_HEIGHT = 1080;
#endif
	window = SDL_CreateWindow("Virtual Sculpting", 0, 0, SCREEN_WIDTH-hackPixels, SCREEN_HEIGHT, SDL_WINDOW_OPENGL | SDL_WINDOW_BORDERLESS
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
		windowRight = SDL_CreateWindow("Virtual Sculpting 2", SCREEN_WIDTH-hackPixels, 0, SCREEN_WIDTH+hackPixels, SCREEN_HEIGHT, SDL_WINDOW_OPENGL | SDL_WINDOW_BORDERLESS);
		rendererRight = SDL_CreateRenderer(windowRight, -1, SDL_RENDERER_ACCELERATED);
		SDL_GetWindowSize(windowRight, &w, &h);
		std::cout << "Right Window created, size: " << w << " x " << h << std::endl;
		checkSDLError(__LINE__);
		contextRight = SDL_GL_CreateContext(windowRight);
		checkSDLError(__LINE__);

		// Post-context creation flags
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
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
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
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
	depth = wantedDepth = 0.0f;

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
				//std::cout << "Key " << (char)e.key.keysym.sym << " pressed" << std::endl;
				//inp->OnKeyPressed(e.key.keysym.sym); //TODO
				/*
				if (e.key.keysym.sym == SDLK_UP){
					wantedDepth += 0.5f;
					//std::cout << "Up pressed" << std::endl;
				}
				else if (e.key.keysym.sym == SDLK_DOWN){
					wantedDepth -= 0.5f;
					//std::cout << "Down pressed" << std::endl;
				}*/
			}
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
//			TranslateMessage(&msg);
//			DispatchMessage(&msg);
		}
}


void SDLContext::SetHeadTracking(StereoKinectHeadTracking* headTracking) {
	this->headTracking = headTracking;

	headTracking->SetDisplaySize(4.0055f,  2.430f);
	headTracking->SetEyeDistance(0.065f);
	headTracking->SetSensorPosition(0.0f, -(2.430f/2.0f - 0.40f), 0.3f);
	headTracking->SetScreemFacing(true);
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
	unsigned int side = model->GetDimm();

	float ratio  = float(SCREEN_WIDTH) / float(SCREEN_HEIGHT);
	
	float viewportHeight = float(side)*3.0f;
	float viewportWidth = viewportHeight*ratio;

	headTracking->SetViewportSize(viewportWidth, viewportHeight);
	headTracking->SetZPlanes(render->ZNEAR, render->ZFAR);	

	headTracking->RetrieveMatrices(leftProj, leftEye, rightProj, rightEye);

	glm::vec3 immersionTranslation (0.0f, 0.0f, depth*side);
	
	glm::mat4 immersiveView = glm::translate(glm::mat4(1.0f), immersionTranslation);

	render->SetProjections(leftProj, rightProj);


	render->Draw(model, _tool_mesh, leftEye*immersiveView, obj, font1, font2, font3, true);

	SDL_GL_SwapWindow(window);

	SDL_GL_MakeCurrent(windowRight, context);

	render->Draw( model, _tool_mesh, rightEye*immersiveView, obj, font1, font2 , font3, false);

	SDL_GL_SwapWindow(windowRight);

	SDL_GL_MakeCurrent(window, context);
#else
	render->Draw(model, _tool_mesh, view, obj, font1, font2, font3);

	SDL_GL_SwapWindow(window);
#endif

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

	depth = depth*(speed - frameTime)/speed + wantedDepth*frameTime/speed;
}

