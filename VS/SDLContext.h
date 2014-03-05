#pragma once
#include "Render.h"
#include "SDL.h"
#include "SDL_opengl.h"
#include "GraphicsLib.h"
#include "Input.h"

class SDLContext
{
public:
	SDLContext();
	~SDLContext();
	void renderScene( GridModel* model, KinectTool* _tool_mesh, glm::mat4& view, 
					  glm::mat4& obj, TextureMappedFont*, TextureMappedFont*, TextureMappedFont* );
	void doMessage();
	void SetInput( Input* );
	bool alive();

private:	
	SDL_Window* window;
	SDL_Renderer* renderer;
	SDL_GLContext context;
	Render* render;
	Input* inp;

	int SCREEN_WIDTH 
		;
	int SCREEN_HEIGHT;

protected:
	bool running;
};

