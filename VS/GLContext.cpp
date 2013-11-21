#include "GLContext.h"


GLContext::GLContext()
{
	#define SCREEN_WIDTH  1920
	#define SCREEN_HEIGHT 1080

	fullscreen = true;

	LPCWSTR title = L"Virtual Sculpting";

	hDC=NULL;		// Private GDI Device Context
	hRC=NULL;		// Permanent Rendering Context
	hWnd=NULL;		// Holds Our Window Handle
	
	// Ask The User Which Screen Mode They Prefer
	if (MessageBox(NULL,L"Would You Like To Run In Fullscreen Mode?", L"Start FullScreen?",MB_YESNO|MB_ICONQUESTION)==IDNO)
		fullscreen = false;							// Windowed Mode

	// Create Our OpenGL Window
	if (!CreateGLWindow("Virtual Sculpting",SCREEN_WIDTH,SCREEN_HEIGHT,16,fullscreen))
		exit(0);									// Quit If Window Was Not Created

	/*// define the screen resolution
	
	//CreateGLWindow();
	
	hWnd = CreateWindowExW(NULL, title, NULL, WS_EX_TOPMOST | WS_POPUP,
				     0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, NULL, NULL, hInstance, NULL);

	
	this->hwnd = hWnd;
	

	create30Context(); // Create a context given a HWND
	
	ShowWindow(this->hwnd, SW_SHOW);
    UpdateWindow(this->hwnd);*/
}

void  GLContext::KillGLWindow(void)								// Properly Kill The Window
{
	if (fullscreen)										// Are We In Fullscreen Mode?
	{
		ChangeDisplaySettings(NULL,0);					// If So Switch Back To The Desktop
		ShowCursor(TRUE);								// Show Mouse Pointer
	}

	if (hRC)											// Do We Have A Rendering Context?
	{
		if (!wglMakeCurrent(NULL,NULL))					// Are We Able To Release The DC And RC Contexts?
		{
			MessageBox(NULL,L"Release Of DC And RC Failed.",L"SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
		}

		if (!wglDeleteContext(hRC))						// Are We Able To Delete The RC?
		{
			MessageBox(NULL,L"Release Rendering Context Failed.",L"SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
		}
		hRC=NULL;										// Set RC To NULL
	}

	if (hDC && !ReleaseDC(hWnd,hDC))					// Are We Able To Release The DC
	{
		MessageBox(NULL,L"Release Device Context Failed.",L"SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
		hDC=NULL;										// Set DC To NULL
	}

	if (hWnd && !DestroyWindow(hWnd))					// Are We Able To Destroy The Window?
	{
		MessageBox(NULL,L"Could Not Release hWnd.",L"SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
		hWnd=NULL;										// Set hWnd To NULL
	}

	if (!UnregisterClass(L"OpenGL",hInstance))			// Are We Able To Unregister Class
	{
		MessageBox(NULL,L"Could Not Unregister Class.",L"SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
		hInstance=NULL;									// Set hInstance To NULL
	}
}

bool GLContext::CreateGLWindow(char* title, int width, int height, int bits, bool fullscreenflag)
{
	GLuint		PixelFormat;			// Holds The Results After Searching For A Match
	WNDCLASS	wc;						// Windows Class Structure
	DWORD		dwExStyle;				// Window Extended Style
	DWORD		dwStyle;				// Window Style
	RECT		WindowRect;				// Grabs Rectangle Upper Left / Lower Right Values
	WindowRect.left=(long)0;			// Set Left Value To 0
	WindowRect.right=(long)width;		// Set Right Value To Requested Width
	WindowRect.top=(long)0;				// Set Top Value To 0
	WindowRect.bottom=(long)height;		// Set Bottom Value To Requested Height

	fullscreen=fullscreenflag;			// Set The Global Fullscreen Flag

	hInstance			= GetModuleHandle(NULL);				// Grab An Instance For Our Window
	wc.style			= CS_HREDRAW | CS_VREDRAW | CS_OWNDC;	// Redraw On Size, And Own DC For Window.
	wc.lpfnWndProc		= (WNDPROC) WndProc;					// WndProc Handles Messages
	wc.cbClsExtra		= 0;									// No Extra Window Data
	wc.cbWndExtra		= 0;									// No Extra Window Data
	wc.hInstance		= hInstance;							// Set The Instance
	wc.hIcon			= LoadIcon(NULL, IDI_WINLOGO);			// Load The Default Icon
	wc.hCursor			= LoadCursor(NULL, IDC_ARROW);			// Load The Arrow Pointer
	wc.hbrBackground	= NULL;									// No Background Required For GL
	wc.lpszMenuName		= NULL;									// We Don't Want A Menu
	wc.lpszClassName	= L"OpenGL";								// Set The Class Name

	if (!RegisterClass(&wc))									// Attempt To Register The Window Class
	{
		MessageBox(NULL,L"Failed To Register The Window Class.",L"ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;											// Return FALSE
	}
	
	if (fullscreen)												// Attempt Fullscreen Mode?
	{
		DEVMODE dmScreenSettings;								// Device Mode
		memset(&dmScreenSettings,0,sizeof(dmScreenSettings));	// Makes Sure Memory's Cleared
		dmScreenSettings.dmSize=sizeof(dmScreenSettings);		// Size Of The Devmode Structure
		dmScreenSettings.dmPelsWidth	= width;				// Selected Screen Width
		dmScreenSettings.dmPelsHeight	= height;				// Selected Screen Height
		dmScreenSettings.dmBitsPerPel	= bits;					// Selected Bits Per Pixel
		dmScreenSettings.dmFields=DM_BITSPERPEL|DM_PELSWIDTH|DM_PELSHEIGHT;

		// Try To Set Selected Mode And Get Results.  NOTE: CDS_FULLSCREEN Gets Rid Of Start Bar.
		if (ChangeDisplaySettings(&dmScreenSettings,CDS_FULLSCREEN)!=DISP_CHANGE_SUCCESSFUL)
		{
			// If The Mode Fails, Offer Two Options.  Quit Or Use Windowed Mode.
			if (MessageBox(NULL,L"The Requested Fullscreen Mode Is Not Supported By\nYour Video Card. Use Windowed Mode Instead?",L"GL",MB_YESNO|MB_ICONEXCLAMATION)==IDYES)
			{
				fullscreen=FALSE;		// Windowed Mode Selected.  Fullscreen = FALSE
			}
			else
			{
				// Pop Up A Message Box Letting User Know The Program Is Closing.
				MessageBox(NULL,L"Program Will Now Close.",L"ERROR",MB_OK|MB_ICONSTOP);
				return FALSE;									// Return FALSE
			}
		}
	}

	if (fullscreen)												// Are We Still In Fullscreen Mode?
	{
		dwExStyle=WS_EX_APPWINDOW;								// Window Extended Style
		dwStyle=WS_POPUP;										// Windows Style
		ShowCursor(FALSE);										// Hide Mouse Pointer
	}
	else
	{
		dwExStyle=WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;			// Window Extended Style
		dwStyle=WS_OVERLAPPEDWINDOW;							// Windows Style
	}

	AdjustWindowRectEx(&WindowRect, dwStyle, FALSE, dwExStyle);		// Adjust Window To True Requested Size

	// Create The Window
	if (!(hWnd=CreateWindowEx(	dwExStyle,							// Extended Style For The Window
								L"OpenGL",							// Class Name
								L"Virtual Sculpting",				// Window Title
								dwStyle |							// Defined Window Style
								WS_CLIPSIBLINGS |					// Required Window Style
								WS_CLIPCHILDREN,					// Required Window Style
								0, 0,								// Window Position
								WindowRect.right-WindowRect.left,	// Calculate Window Width
								WindowRect.bottom-WindowRect.top,	// Calculate Window Height
								NULL,								// No Parent Window
								NULL,								// No Menu
								hInstance,							// Instance
								NULL)))								// Dont Pass Anything To WM_CREATE
	{
		KillGLWindow();												// Reset The Display
		MessageBox(NULL,L"Window Creation Error.",L"ERROR",MB_OK|MB_ICONEXCLAMATION);
		return false;												// Return FALSE
	}
	
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	printf("Creating a 3D context... \n");

	running = true;
	render = NULL;
	//hdc = GetDC(hWnd);


	static	PIXELFORMATDESCRIPTOR pfd=				// pfd Tells Windows How We Want Things To Be
	{
		sizeof(PIXELFORMATDESCRIPTOR),				// Size Of This Pixel Format Descriptor
		1,											// Version Number
		PFD_DRAW_TO_WINDOW |						// Format Must Support Window
		PFD_SUPPORT_OPENGL |						// Format Must Support OpenGL
		PFD_DOUBLEBUFFER,							// Must Support Double Buffering
		PFD_TYPE_RGBA,								// Request An RGBA Format
		bits,										// Select Our Color Depth
		0, 0, 0, 0, 0, 0,							// Color Bits Ignored
		0,											// No Alpha Buffer
		0,											// Shift Bit Ignored
		0,											// No Accumulation Buffer
		0, 0, 0, 0,									// Accumulation Bits Ignored
		16,											// 16Bit Z-Buffer (Depth Buffer)  
		0,											// No Stencil Buffer
		0,											// No Auxiliary Buffer
		PFD_MAIN_PLANE,								// Main Drawing Layer
		0,											// Reserved
		0, 0, 0										// Layer Masks Ignored
	};
	
	if (!(hDC=GetDC(hWnd)))							// Did We Get A Device Context?
	{
		KillGLWindow();								// Reset The Display
		MessageBox(NULL,L"Can't Create A GL Device Context.",L"ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;								// Return FALSE
	}

	if (!(PixelFormat=ChoosePixelFormat(hDC,&pfd)))	// Did Windows Find A Matching Pixel Format?
	{
		KillGLWindow();								// Reset The Display
		MessageBox(NULL,L"Can't Find A Suitable PixelFormat.",L"ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;								// Return FALSE
	}

	if(!SetPixelFormat(hDC,PixelFormat,&pfd))		// Are We Able To Set The Pixel Format?
	{
		KillGLWindow();								// Reset The Display
		MessageBox(NULL,L"Can't Set The PixelFormat.",L"ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;								// Return FALSE
	}
	/*
	if (!(hRC=wglCreateContext(hDC)))				// Are We Able To Get A Rendering Context?
	{
		KillGLWindow();								// Reset The Display
		MessageBox(NULL,L"Can't Create A GL Rendering Context.",L"ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;								// Return FALSE
	}

	if(!wglMakeCurrent(hDC,hRC))					// Try To Activate The Rendering Context
	{
		KillGLWindow();								// Reset The Display
		MessageBox(NULL,L"Can't Activate The GL Rendering Context.",L"ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;								// Return FALSE
	}
	*/

	HGLRC tempOGLWidget = wglCreateContext(hdc);
	wglMakeCurrent(hdc, tempOGLWidget);
	int attributes[] = 
	{
		WGL_CONTEXT_MAJOR_VERSION_ARB, 4,
		WGL_CONTEXT_MINOR_VERSION_ARB, 1,
		WGL_CONTEXT_FLAGS_ARB, WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB, 
		0
	};
	if (wglewIsSupported("WGL_ARB_create_context") == 1) 
	{
		hrc = wglCreateContextAttribsARB(hdc, NULL, attributes);
		wglMakeCurrent(NULL, NULL);
		wglDeleteContext(tempOGLWidget);
		wglMakeCurrent(hdc, hrc);
	}
	else 
		hrc = tempOGLWidget; 

	int glVersion[2] = {-1, -1}; 

	glGetIntegerv(GL_MAJOR_VERSION, &glVersion[0]);
	glGetIntegerv(GL_MINOR_VERSION, &glVersion[1]);

	// Output which version of OpenGL we are using
	std::cout << "Using OpenGL: " << glVersion[0] << "." << glVersion[1] << std::endl; 

	render = new Render();
	render->Init();
	render->Resize( height, width );
	
	// Changes an attribute of the specified window. 
	//The function also sets a value at the specified offset in the extra window memory.
	SetWindowLongPtr( hWnd, GWLP_USERDATA, reinterpret_cast<long>(render));


	ShowWindow(this->hWnd,SW_SHOW);					// Show The Window
	//SetForegroundWindow(this->hWnd);					// Slightly Higher Priority
	SetFocus(this->hWnd);							// Sets Keyboard Focus To The Window
	
	//ReSizeGLScene(width, height);					// Set Up Our Perspective GL Screen
	
    //UpdateWindow(this->hWnd);
	return true;									// Success
}
/*
void GLContext::ReSizeGLScene(GLsizei width, GLsizei height)		// Resize And Initialize The GL Window
{
	if (height==0)										// Prevent A Divide By Zero By
	{
		height=1;										// Making Height Equal One
	}

	glViewport(0,0,width,height);						// Reset The Current Viewport

	glMatrixMode(GL_PROJECTION);						// Select The Projection Matrix
	glLoadIdentity();									// Reset The Projection Matrix

	// Calculate The Aspect Ratio Of The Window
	//gluPerspective(45.0f,(GLfloat)width/(GLfloat)height,0.1f,100.0f);
	render->Resize( height, width );


	glMatrixMode(GL_MODELVIEW);							// Select The Modelview Matrix
	glLoadIdentity();									// Reset The Modelview Matrix
}*/

Render *GLContext::GetObjectFromHWnd( HWND hWnd )
{
	return reinterpret_cast<Render*>(GetWindowLongPtr(hWnd,GWLP_USERDATA));
}

LRESULT CALLBACK GLContext::WndProc(HWND hWnd,		// Handle For This Window
									UINT message,	// Message For This Window
									WPARAM wParam,	// Additional Message Information
									LPARAM lParam)	// Additional Message Information
{
	Render *pWnd = GetObjectFromHWnd( hWnd );
	switch (message) {
		case WM_SYSCOMMAND:							// Intercept System Commands
		{
			switch (wParam)							// Check System Calls
			{
				case SC_SCREENSAVE:					// Screensaver Trying To Start?
				case SC_MONITORPOWER:				// Monitor Trying To Enter Powersave?
				return 0;							// Prevent From Happening
			}
			break;									// Exit
		}
		case WM_CLOSE:								// Did We Receive A Close Message?
		{
			PostQuitMessage(0);						// Send A Quit Message
			return 0;								// Jump Back
		}
		case WM_SIZE: // If our window is resizing
		{
			pWnd->Resize(LOWORD(lParam), HIWORD(lParam)); // Send the new window size to our OGLWidget
			break;
		}
		case WM_DESTROY:
		{
			PostQuitMessage(0);
			break;
		}
	}

	// Pass All Unhandled Messages To DefWindowProc
	return DefWindowProc(hWnd, message, wParam, lParam);
}

void GLContext::SetInput( Input* input)
{
	inp = input;
}

GLContext::GLContext(HWND hwnd) 
{
	this->hWnd = hwnd;
	create30Context(); // Create a context given a HWND
	
	ShowWindow(this->hWnd, SW_SHOW);
    UpdateWindow(this->hWnd);
}

GLContext::~GLContext() 
{
	delete render;
    wglMakeCurrent(hdc, 0);
    wglDeleteContext(hrc);

    ReleaseDC(hWnd, hdc);	
}

void GLContext::doMessage()
{
	while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) 
	{
		if (msg.message == WM_QUIT) 
		{
			running = false;
		}
		else 
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
		DispatchMessage(&msg);
	}
}


bool GLContext::alive()
{
	return running;
}

void GLContext::renderScene( GridModel* model, KinectTool* _tool_mesh, glm::mat4& view, glm::mat4& obj )
{	
	render->Draw( model, _tool_mesh, view, obj);
	
	SwapBuffers(hdc);
}

bool GLContext::create30Context() 
{
	printf("Creating a 3D context... \n");

	running = true;
	render = NULL;
	hdc = GetDC(hWnd);

	PIXELFORMATDESCRIPTOR pfd;
	memset(&pfd, 0, sizeof(PIXELFORMATDESCRIPTOR)); 
	pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
	pfd.dwFlags = PFD_DOUBLEBUFFER | PFD_SUPPORT_OPENGL;// | PFD_DRAW_TO_WINDOW;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 32;
	pfd.cDepthBits = 32;
	pfd.iLayerType = PFD_MAIN_PLANE;

	int nPixelFormat = ChoosePixelFormat(hdc, &pfd);
	if (nPixelFormat == 0)
		return false;

	int bResult = SetPixelFormat(hdc, nPixelFormat, &pfd);
	if (!bResult)
		return false;

	HGLRC tempOGLWidget = wglCreateContext(hdc);
	wglMakeCurrent(hdc, tempOGLWidget);

	GLenum error = glewInit();
	if (error != GLEW_OK)
		return false;		 

	int attributes[] = 
	{
		WGL_CONTEXT_MAJOR_VERSION_ARB, 4,
		WGL_CONTEXT_MINOR_VERSION_ARB, 1,
		WGL_CONTEXT_FLAGS_ARB, WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB, 
		0
	};

	if (wglewIsSupported("WGL_ARB_create_context") == 1) 
	{
		hrc = wglCreateContextAttribsARB(hdc, NULL, attributes);
		wglMakeCurrent(NULL, NULL);
		wglDeleteContext(tempOGLWidget);
		wglMakeCurrent(hdc, hrc);
	}
	else 
	{
		hrc = tempOGLWidget; 
	}

	int glVersion[2] = {-1, -1}; 

	glGetIntegerv(GL_MAJOR_VERSION, &glVersion[0]);
	glGetIntegerv(GL_MINOR_VERSION, &glVersion[1]);

	// Output which version of OpenGL we are using
	std::cout << "Using OpenGL: " << glVersion[0] << "." << glVersion[1] << std::endl; 
	
	render = new Render();
	render->Init();
	render->Resize( 1920, 1080 );

	// Changes an attribute of the specified window. 
	//The function also sets a value at the specified offset in the extra window memory.
	SetWindowLongPtr( hWnd, GWLP_USERDATA, reinterpret_cast<long>(render));

	return true; // We have successfully created a context, return true
}
