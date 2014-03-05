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
	/*LPCWSTR title = L"Virtual Sculpting";

	WNDCLASSW windowClass;
	HWND hWnd;
	DWORD dwExStyle = WS_EX_APPWINDOW ;

	hInstance = GetModuleHandle(NULL);

	windowClass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	windowClass.lpfnWndProc = (WNDPROC) WndProc;
	windowClass.cbClsExtra = 0;
	windowClass.cbWndExtra = 0;
	windowClass.hInstance = hInstance;
	windowClass.hIcon = LoadIcon(NULL, IDI_WINLOGO);
	windowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	windowClass.hbrBackground = NULL;
	windowClass.lpszMenuName = NULL;
	windowClass.lpszClassName = title;

	if (!RegisterClassW(&windowClass)) {
		return;
	}*/

	SDL_Init(SDL_INIT_EVERYTHING);
	checkSDLError(__LINE__);

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
	SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 32); // Lel
	checkSDLError(__LINE__);

	SCREEN_WIDTH = 800;
	SCREEN_HEIGHT = 600;
	SDL_CreateWindowAndRenderer(SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_OPENGL /*| SDL_WINDOW_FULLSCREEN_DESKTOP*/, &window, &renderer);
	SDL_GetWindowSize(window, &SCREEN_WIDTH, &SCREEN_HEIGHT);
	std::cout << "Window created, size: " << SCREEN_WIDTH << " x " << SCREEN_HEIGHT << std::endl;
	checkSDLError(__LINE__);

	running = true;
	render = NULL;

	context = SDL_GL_CreateContext(window);
	checkSDLError(__LINE__);

	GLenum error = glewInit();
	if (error != GLEW_OK) {
		std::cout << "FATAL: Could not init GLEW" << std::endl;
		std::cout << "Error: " << glewGetErrorString(error) << std::endl;
	}

	/*int attributes[] = 
	{
		WGL_CONTEXT_MAJOR_VERSION_ARB, 4,
		WGL_CONTEXT_MINOR_VERSION_ARB, 1,
		WGL_CONTEXT_FLAGS_ARB, WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB, 
		0
	};

	if (glewIsSupported("GL_ARB_create_context") == 1) 
	{
		wglCreateContextAttribsARB(hdc, NULL, attributes);
		wglMakeCurrent(NULL, NULL);
		wglDeleteContext(tempOGLWidget);
		wglMakeCurrent(hdc, hrc);
	}
	else 
	{
		hrc = tempOGLWidget; 
	}*/

	if (glewIsSupported("GL_ARB_create_context") == 1) {
		std::cout << "ARB context creation supported" << std::endl;
	}

	int glVersion[2] = {-1, -1}; 
	glGetIntegerv(GL_MAJOR_VERSION, &glVersion[0]);
	glGetIntegerv(GL_MINOR_VERSION, &glVersion[1]);

	std::cout << "Using OpenGL: " << glVersion[0] << "." << glVersion[1] << std::endl; // Output which version of OpenGL we are using
	
	render = new Render();
	render->Init();
	render->Resize( SCREEN_WIDTH, SCREEN_HEIGHT );

	//ShowWindow(this->hwnd, SW_SHOW);
    //UpdateWindow(this->hwnd);
	std::cout << "SDLContext initialized" << std::endl;
}

void SDLContext::SetInput( Input* input)
{
	inp = input;
}

SDLContext::~SDLContext() 
{
	SDL_GL_DeleteContext(context);
	SDL_Quit();
	delete render;
}


void SDLContext::doMessage()
{
	SDL_Event e;
	while (SDL_PollEvent(&e) != 0) 
	{
			if (e.type == SDL_QUIT) 
			{
				running = false;
			}
			/*else 
			{
				if ( msg.message == WM_KEYDOWN )
				{
					UINT8 c = MapVirtualKey(msg.wParam ,MAPVK_VK_TO_CHAR);					
					inp->OnKeyPressed(c);
				}
				else if ( msg.message == WM_LBUTTONDOWN )
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
				}
			}
			TranslateMessage(&msg);
			DispatchMessage(&msg);*/
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
}

