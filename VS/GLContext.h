#pragma once
#include "Render.h"
#include "GraphicsLib.h"
#include "Input.h"

class GLContext
{
public:
	GLContext();
	~GLContext();
	void renderScene(GridModel* model, KinectTool* _tool_mesh, glm::mat4& view, glm::mat4& obj );
	void doMessage();
	void SetInput( Input* );
	bool alive();

	void setupScene();

	bool		fullscreenflag;

private:	
	bool create30Context();
	GLContext(HWND hwnd);

	static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
	static Render* GetObjectFromHWnd( HWND hWnd );
	Render* render;
	Input* inp;

	HDC	 hDC;		// Private GDI Device Context
	HGLRC hRC;		// Permanent Rendering Context
	HWND hWnd;		// Window identifier


	bool	fullscreen;	// Fullscreen Flag Set To Fullscreen Mode By Default

	bool CreateGLWindow(char*, int, int, int, bool );
	void  KillGLWindow(void);

	void ReSizeGLScene(GLsizei, GLsizei);

protected:
	HINSTANCE hInstance;
	HGLRC hrc; // Rendering context
	HDC hdc; // Device context
	MSG msg;
	bool running;
};

