#include "SDLContext.h"


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

	// Main Window
#ifdef VIC4K
	SCREEN_WIDTH = 4096;
	SCREEN_HEIGHT = 2400;
#else
	SCREEN_WIDTH = 1920;
	SCREEN_HEIGHT = 1080;
#endif
	window = SDL_CreateWindow("Virtual Sculpting", 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_OPENGL | SDL_WINDOW_BORDERLESS
#ifdef VIC4K
		| SDL_WINDOW_FULLSCREEN_DESKTOP
#endif
		);
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	//SDL_CreateWindowAndRenderer(SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_OPENGL | SDL_WINDOW_BORDERLESS, &window, &renderer);
	SDL_GetWindowSize(window, &SCREEN_WIDTH, &SCREEN_HEIGHT);
	std::cout << "Main Window created, size: " << SCREEN_WIDTH << " x " << SCREEN_HEIGHT << std::endl;
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
		//SDL_CreateWindowAndRenderer(SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_OPENGL | SDL_WINDOW_BORDERLESS, &window, &renderer);
		SDL_GetWindowSize(windowRight, &SCREEN_WIDTH, &SCREEN_HEIGHT);
		std::cout << "Right Window created, size: " << SCREEN_WIDTH << " x " << SCREEN_HEIGHT << std::endl;
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
		SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 32); // Lel
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
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 32); // Lel
	checkSDLError(__LINE__);

	running = true;
	render = NULL;

	int glVersion[2] = {-1, -1}; 
	glGetIntegerv(GL_MAJOR_VERSION, &glVersion[0]);
	glGetIntegerv(GL_MINOR_VERSION, &glVersion[1]);

	std::cout << "Using OpenGL: " << glVersion[0] << "." << glVersion[1] << std::endl; // Output which version of OpenGL we are using
	
	render = new Render();
	render->Init();
	render->Resize( SCREEN_WIDTH, SCREEN_HEIGHT );

	//ShowWindow(this->hwnd, SW_SHOW);
    //UpdateWindow(this->hwnd);

	FPS = 1.0f;
	lastTick = SDL_GetTicks();
	lastFPSTick = lastTick;

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
					std::cout << "Key " << (char)e.key.keysym.sym << " pressed" << std::endl;
					inp->OnKeyPressed(e.key.keysym.sym);
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


bool SDLContext::alive()
{
	return running;
}

void SDLContext::renderScene( GridModel* model, KinectTool* _tool_mesh, 
							glm::mat4& view, glm::mat4& obj, TextureMappedFont* font1, 
							TextureMappedFont* font2, TextureMappedFont* font3)
{
	render->Draw( model, _tool_mesh, view, obj, font1, font2 , font3);

	SDL_GL_SwapWindow(window);

#if (STEREO > 0)
		SDL_GL_MakeCurrent(windowRight, context);

		render->Draw( model, _tool_mesh, view, obj, font1, font2 , font3);

		SDL_GL_SwapWindow(windowRight);

		SDL_GL_MakeCurrent(window, context);
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
}

