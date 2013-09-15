#include <time.h>
#include "GridModel.h"
#include "GLContext.h"
#include "KinectTool.h"
#include "TriangleMesh.h"
#include "Soundify.h"


/**
 * Calculate the difference, in milliseconds, between two time points
 * 
 * @param   end    The later time point
 * @param   start  The earlier time point
 * @return         The difference in milliseconds
 */
inline double diffclock(clock_t end, clock_t start)
{
	return (end - start) * 1000. / CLOCKS_PER_SEC;
}


/**
 * Main method
 * 
 * @param   argc  The number of command line arguments
 * @param   argv  Command line arguments
 * @return        Exit value, zero on success
 */
int main(int argc, char** argv)
{
	int          acted         = 0;
	bool         space_pressed = false;
	GLContext*   graphics;
	Input*       input;
	GridModel*   model;
	unsigned int side;
	KinectTool*  kinect;
	
	(void) argc;
	(void) argv;
	
	
	/* Initialise GUI */
	graphics = new GLContext();
	input    = new Input();
	graphics->input = input;
	
	/* Initialise model */
	model = new GridModel(8); /* power of 2 */
	side  = model->GetDimm();
	input->SetZoom(-(side * 4.0f));
	input->SetModel(model);
	
	/* Initialise Kinect */
	#define SIDE  (side * 0.75f)
	kinect = new KinectTool(SIDE, SIDE, SIDE, -SIDE);
	#undef SIDE
	
	/* Initialise audio */
	initialise_audio();
	
	
	/* Main loop */
	while (graphics->running)
	{
		#ifdef DEBUG_TIME
			clock_t start = clock(), end;
		#endif
		
		input   ->UpdateFrame();
		graphics->doMessage();
		kinect  ->DoToolUpdate();
		
		acted = (space_pressed ^= input->IsPressed(' '))
			? kinect->InteractModel(model, input->GetObjectQ())
			: 0;
		
		model->UpdateGrid();
		graphics->renderScene(model, kinect, input->GetViewM(), input->GetObjectM());
		
		if (acted)
			audio_set_pitch(0.1f + glm::log2(acted * 1.0f) / 1000.0f);
		audio_set_gain(acted ? 1.0f : 0.0f);
		
		#ifdef DEBUG_TIME
			end = clock();
			std::cerr << "Frame time = " << diffclock(end, start) << " ms,  "
				  << "Interacted: " << acted << std::endl;
		#endif
	}
	
	
	terminate_audio();
	delete model;
	delete input;
	delete kinect;
	delete graphics;
	
	return 0;
}

