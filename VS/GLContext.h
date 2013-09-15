#pragma once
#include "Render.h"
#include "GraphicsLib.h"
#include "Input.h"

class GLContext
{
public:
	/**
	 * Constructor
	 */
	GLContext();
	
	/**
	 * Destructor
	 */
	~GLContext();
	
	void renderScene(GridModel* model, KinectTool* _tool_mesh, glm::mat4 view, glm::mat4 obj);
	void doMessage();
	
	Input* input;
	bool running;
	
	void setupScene();
	
	//void setObject(glm::mat4& obj);
	//void setProjection(glm::mat4& obj);
	
private:
	bool create30Context();
	
	static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
	static Render* GetObjectFromHWnd(HWND hWnd);
	Render* render;
	
protected:
	HINSTANCE hInstance;
	HGLRC hrc; // Rendering context
	HDC hdc; // Device context
	HWND hwnd; // Window identifier
	MSG msg;
};

