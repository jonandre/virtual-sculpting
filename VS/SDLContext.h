#ifndef SDL_CONTEXT_H
#define SDL_CONTEXT_H

#define VIC4K

#ifdef VIC4K
	#define STEREO 1
#else
	#define STEREO 1
#endif

#if (STEREO > 0)
	#include "StereoRender.h"
#else
	#include "Render.h"
#endif

#include "StereoKinectHeadTracking.h"
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
	void SetHeadTracking(StereoKinectHeadTracking* headTracking);
	bool alive();

private:	
	SDL_Window* window;
	SDL_Renderer* renderer;
	SDL_GLContext context;

	SDL_Window* windowRight;
	SDL_Renderer* rendererRight;
	SDL_GLContext contextRight;

#if (STEREO > 0)
	StereoRender* render;

	glm::mat4 leftProj, rightProj, leftEye, rightEye;
#else
	Render* render;
#endif

	StereoKinectHeadTracking* headTracking;

	Input* inp;

	int SCREEN_WIDTH;
	int SCREEN_HEIGHT;

	int lastTick;
	int lastFPSTick;
	static const int FPSPeriod = 1000;
	float FPS;

	// PROVISIONAL

	float depth;
	float speed;
	float wantedDepth;

protected:
	bool running;
};

#endif // SDL_CONTEXT_H
