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
	(void) argc;
	(void) argv;
	
	GLContext* cntx = new GLContext();
	Input* inp = new Input();
	cntx->SetInput(inp);
	
	unsigned int power = 8; // power of 2
	
	GridModel* model = new GridModel(power);
	unsigned int side = model->GetDimm();
	inp->SetZoom(-(side * 4.0f));
	inp->SetModel(model);
	
	KinectTool* tool = new KinectTool(side * 0.75f, side * 0.75f, side * 0.75f, -(side * 0.75f));
	
	initialise_audio();
	
	int acted = 0;
	bool space_pressed = false;
	while (cntx->alive())
	{
		#ifdef DEBUG_TIME
			clock_t start = clock();
		#endif
		
		inp->UpdateFrame();
		cntx->doMessage();
		
		
		tool->DoToolUpdate();
		
		space_pressed ^= inp->IsPressed(' ');
		
		acted = space_pressed ? tool->InteractModel(model, inp->GetObjectQ()) : 0;
		
		model->UpdateGrid();
		cntx->renderScene(model, tool, inp->GetViewM(), inp->GetObjectM());
		
		if (acted)
			audio_set_pitch(0.1f + glm::log2(acted * 1.0f) / 1000.0f);
		audio_set_gain(acted ? 1.0f : 0.0f);
		
		#ifdef DEBUG_TIME
			clock_t end = clock();
			std::cerr << "Frame time = " << diffclock(end, start) << " ms, "
				  << " Interacted: " << acted << std::endl;
		#endif
	}
	
	terminate_audio();
	delete model;
	delete inp;
	delete tool;
	delete cntx;
	return 0;
}

