#include "GLContext.h"


/**
 * Constructor
 */
GLContext::GLContext()
{
	/* Create window */
	LPCWSTR   title     = (LPCWSTR)L"Virtual Sculpting";
	DWORD     dwExStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;
	HINSTANCE hInstance = GetModuleHandle(NULL);
	WNDCLASSW windowClass;
	
	memset(&windowClass, 0, sizeof(WNDCLASSW));
	windowClass.style         = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	windowClass.lpfnWndProc   = (WNDPROC)WndProc;
	windowClass.hInstance     = hInstance;
	windowClass.hIcon         = LoadIcon(NULL, IDI_WINLOGO);
	windowClass.hCursor       = LoadCursor(NULL, IDC_ARROW);
	windowClass.lpszClassName = title;
	
	if (RegisterClassW(&windowClass) == false)
		return;
	
	this->hwnd = CreateWindowExW(dwExStyle, title, title, WS_OVERLAPPEDWINDOW,
				     CW_USEDEFAULT, 0, 512, 512, NULL, NULL, hInstance, NULL);
	
	
	/* Create 3D */
	Create3DContext();
	
	/* Display window */
	ShowWindow(this->hwnd, SW_SHOW);
	UpdateWindow(this->hwnd);
}


/**
 * Destructor
 */
GLContext::~GLContext()
{
	delete this->render;
	wglMakeCurrent(this->device, 0);
	wglDeleteContext(this->rendering);
	
	ReleaseDC(this->hwnd, this->device);
}


LRESULT CALLBACK GLContext::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	Render* pWnd = reinterpret_cast<Render*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
	
	switch (message)
	{
		case WM_SIZE:
			pWnd->Resize(LOWORD(lParam), HIWORD(lParam));
			break;
			
		case WM_DESTROY:
			PostQuitMessage(0);
			break;
	}
	
	return DefWindowProc(hWnd, message, wParam, lParam);
}
 

/**
 * Initialise OpenGL context
 */
void GLContext::Create3DContext()
{
	PIXELFORMATDESCRIPTOR pixelFormatDesciptor;
	int pixelFormat;
	HGLRC widget;
	
	this->running = true;
	this->render = NULL;
	this->device = GetDC(this->hwnd);
	
	
	/* Configure OpenGL context */
	memset(&pixelFormatDesciptor, 0, sizeof(PIXELFORMATDESCRIPTOR));
	pixelFormatDesciptor.nSize      = sizeof(PIXELFORMATDESCRIPTOR);
	pixelFormatDesciptor.dwFlags    = PFD_DOUBLEBUFFER | PFD_SUPPORT_OPENGL; // | PFD_DRAW_TO_WINDOW;
	pixelFormatDesciptor.iPixelType = PFD_TYPE_RGBA;
	pixelFormatDesciptor.cColorBits = 32;
	pixelFormatDesciptor.cDepthBits = 32;
	pixelFormatDesciptor.iLayerType = PFD_MAIN_PLANE;
	
	if ((pixelFormat = ChoosePixelFormat(this->device, &pixelFormatDesciptor)) == 0)  return;
	if (SetPixelFormat(this->device, pixelFormat, &pixelFormatDesciptor)       == 0)  return;
	
	
	widget = wglCreateContext(this->device);
	wglMakeCurrent(this->device, widget);
	
	if (glewInit() != GLEW_OK)
		return;
	
	if (wglewIsSupported("WGL_ARB_create_context") == 1)
	{
		int attributes[] = {
			WGL_CONTEXT_MAJOR_VERSION_ARB, 4,
			WGL_CONTEXT_MINOR_VERSION_ARB, 1,
			WGL_CONTEXT_FLAGS_ARB, WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB,
			0};
		
		this->rendering = wglCreateContextAttribsARB(this->device, NULL, attributes);
		wglMakeCurrent(NULL, NULL);
		wglDeleteContext(widget);
		wglMakeCurrent(this->device, this->rendering);
	}
	else
		this->rendering = widget;
	
	
	/* Print OpenGL version  */
	int glVersion[2] = {-1, -1};
	glGetIntegerv(GL_MAJOR_VERSION, &(glVersion[0]));
	glGetIntegerv(GL_MINOR_VERSION, &(glVersion[1]));
	std::cerr << "Using OpenGL: " << glVersion[0] << "." << glVersion[1] << std::endl;
	
	
	this->render = new Render();
	this->render->Init();
	
	SetWindowLongPtr(this->hwnd, GWLP_USERDATA, reinterpret_cast<long>(this->render));
}


/**
 * Read all input messages
 */
void GLContext::DoMessage()
{
	MSG msg;
	while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
	{
		if      (msg.message == WM_QUIT)         this->running = false;
		else if (msg.message == WM_KEYDOWN)      input->OnKeyPressed(MapVirtualKey(msg.wParam, MAPVK_VK_TO_CHAR));
		else if (msg.message == WM_LBUTTONDOWN)  input->OnMouseLBDown(msg.pt.x, msg.pt.y);
		else if (msg.message == WM_LBUTTONUP)    input->OnMouseLBUp(msg.pt.x, msg.pt.y);
		else if (msg.message == WM_MOUSEMOVE)    input->OnMouseMove(msg.pt.x, msg.pt.y);
		else if (msg.message == WM_MOUSEWHEEL)   input->OnScroll(GET_WHEEL_DELTA_WPARAM(msg.wParam));
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}


void GLContext::RenderScene(GridModel* model, KinectTool* tool_mesh, glm::mat4 view, glm::mat4 obj)
{
	render->Draw(model, tool_mesh, view, obj);
	SwapBuffers(this->device);
}

