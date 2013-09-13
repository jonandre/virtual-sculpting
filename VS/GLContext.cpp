#include "GLContext.h"


GLContext::GLContext()
{
	LPCWSTR title = L"Virtual Sculpting";

	WNDCLASSW windowClass;
	HWND hWnd;
	DWORD dwExStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;

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
	}

	hWnd = CreateWindowExW(dwExStyle, title, title, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, 512, 512, NULL, NULL, hInstance, NULL);

	this->hwnd = hWnd;

	create30Context(); // Create a context given a HWND
	
	ShowWindow(this->hwnd, SW_SHOW);
    UpdateWindow(this->hwnd);
}

Render *GLContext::GetObjectFromHWnd( HWND hWnd )
{
	return reinterpret_cast<Render*>(GetWindowLongPtr(hWnd,GWLP_USERDATA));
}
LRESULT CALLBACK GLContext::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) 
{
	Render *pWnd = GetObjectFromHWnd( hWnd );
	switch (message) {
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

	return DefWindowProc(hWnd, message, wParam, lParam);
}

void GLContext::SetInput( Input* input)
{
	inp = input;
}

GLContext::GLContext(HWND hwnd) 
{
	this->hwnd = hwnd;
	create30Context(); // Create a context given a HWND
	
	ShowWindow(this->hwnd, SW_SHOW);
    UpdateWindow(this->hwnd);
}

GLContext::~GLContext() 
{
	delete render;
    wglMakeCurrent(hdc, 0);
    wglDeleteContext(hrc);

    ReleaseDC(hwnd, hdc);	
}

bool GLContext::create30Context() 
{
	running = true;
	render = NULL;
	hdc = GetDC(hwnd);

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

	std::cout << "Using OpenGL: " << glVersion[0] << "." << glVersion[1] << std::endl; // Output which version of OpenGL we are using
	
	render = new Render();
	render->Init();

	SetWindowLongPtr( hwnd, GWLP_USERDATA, reinterpret_cast<long>(render));

	return true; // We have successfully created a context, return true
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
	render->Draw( model, _tool_mesh, view, obj );
	
	SwapBuffers(hdc);
}

