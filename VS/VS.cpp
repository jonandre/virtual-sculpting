#include "GridModel.h"
#include "GLContext.h"
#include <time.h>
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
    return (end - start) * 1000 / CLOCKS_PER_SEC;
}


int main(int argc, UINT8** argv) 
{
	GLContext* cntx = new GLContext();//Window+render.
	Input* inp = new Input();//Input system.
	cntx->SetInput( inp );//Context redirects mouse+keyb to Input

	unsigned int power = 8;

	GridModel* model = new GridModel(power);//power of 2
	unsigned int side = model->GetDimm();
	inp->SetZoom(-(side*4.0f));
	inp->SetModel( model );

	KinectTool* tool = new KinectTool( (side*0.75f), (side*0.75f), side*.75f, -(side*.75f));

	Soundify snd;
	snd.Play();

	int acted = 0;
	bool space_pressed = false;
	while (cntx->alive())
	{	
		clock_t start = clock();
		acted = 0;

		inp->UpdateFrame();//Reset frame variables.
		cntx->doMessage();//Win message loop


		tool->DoToolUpdate();//update tool state - like depthmap
		
		if ( inp->IsPressed(' ') )
		{
			space_pressed = !space_pressed;
		}

		if ( space_pressed )
			acted = tool->InteractModel( model, inp->GetObjectQ());//obvious
		
		model->UpdateGrid();// update visual representation of model
		cntx->renderScene(model, tool, inp->GetViewM(), inp->GetObjectM());// do actual rendering.
		
		///
		//static int tmp = 0;
		//tmp++;
		//acted = (rand()%100) > 10 ? (rand()%3000) : 0;
		//acted = tmp;
		///

		if (acted)//sonification
		{
			snd.SetGain(1.0f);
			snd.SetPitch( 0.1 + glm::log2(acted*1.0f)/1000.0f );
		}
		else
		{
			snd.SetGain(0.0f);
		}

		clock_t end = clock();
		//std::cout<<"Frame time = " << diffclock( end, start )<< " ms, " << "Interacted: "<< acted << std::endl;
	}

	snd.SetGain(0.0f);

	delete model;
	delete inp;
	delete tool;
	delete cntx;
	return 0;
}

