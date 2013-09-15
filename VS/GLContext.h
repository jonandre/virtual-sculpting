#pragma once
#include "Render.h"
#include "GraphicsLib.h"
#include "Input.h"


/**
 * Window classs
 */
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
	
	
	/**
	 * Read all input messages
	 */
	void DoMessage();
	
	void RenderScene(GridModel* model, KinectTool* tool_mesh, glm::mat4 view, glm::mat4 obj);
	
	
	/**
	 * Object to store input information in
	 */
	Input* input;
	
	/**
	 * Whether the window still exists
	 */
	bool running;
	
	
private:
	/**
	 * Initialise OpenGL context
	 */
	void Create3DContext();
	
	static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
	
	
	Render* render;
	
	/**
	 * Rendering context
	 */
	HGLRC rendering;
	
	/**
	 * Device context
	 */
	HDC device;
	
	/**
	 * Window identifier
	 */
	HWND hwnd;
};

