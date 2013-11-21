#include "Render.h"
#include "GraphicsLib.h"
#include "GridModel.h"
#include "VAO.h"
#include "KinectTool.h"
#include "TriangleMesh.h"
#include <time.h>

#include "VSText.h"


Render::Render(): shader(NULL)
{
	printf("Rendering session started. \n");
	windowWidth = 1920;
	windowHeight = 1080;
	Point center;
	center.coord[0] = 0.0;
	center.coord[1] = 0.0;
	center.coord[2] = 0.0;
	Color clr;
	clr.comp[0] = 255;
	clr.comp[1] = 255;
	clr.comp[2] = 255;
	clr.comp[3] = 255;
	
	text = new VSText();
}


Render::~Render()
{
	if (shader)
		delete shader;
}

void Render::Init()
{
	printf("Initializing render... \n");
	SetupScene();
}
/**
 *	Sets the scene to black
 *	Specifies how the red, green, blue, and alpha destination blending factors are computed
 *  Loads chaders
 *  Setting up the camera focalLength aspect Depth
 */
void Render::SetupScene()
{
	printf("Setting up render scene... \n");
	
	glClearColor(0.0f, 0.0f, 0.0f, 0.5f);
	
	// Specifies how the red, green, blue, and alpha destination blending factors are computed.
	// GL_ONE_MINUS_SRC_ALPHA
	printf("Gl blend function initiated with gl source alfa and gl one minus src alpha. \n");
	glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	// Enables blend
	glEnable(GL_DEPTH_TEST);
	
	// Loads chaders
	printf("Loads shaders... \n");
	shader = new Shader();
	printf("Loads Fragnt shader...");
	shader->loadFragmentShader("Shaders/shader.frag");
	printf("Loads Fragnt geometry...");
	shader->loadGeometryShader("Shaders/shader.geom");
	printf("Loads Fragnt vertex...");
	shader->loadVertexShader("Shaders/shader.vert");
	printf("Links shaders... \n");
	shader->link();
	
	// 
	pMatrixLocation = glGetUniformLocation(shader->id(), "p");
	mMatrixLocation = glGetUniformLocation(shader->id(), "m");
	vMatrixLocation = glGetUniformLocation(shader->id(), "v");
	
	// Setting up the camera focalLength aspect Depth
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	glShadeModel(GL_SMOOTH);							// Enable Smooth Shading
	glClearColor(0.0f, 0.0f, 0.0f, 0.5f);				// Black Background
	glClearDepth(1.0f);									// Depth Buffer Setup
	glEnable(GL_DEPTH_TEST);							// Enables Depth Testing
	glDepthFunc(GL_LEQUAL);								// The Type Of Depth Testing To Do
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);	// Really Nice Perspective Calculations
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	printf("Creats a projection matrix for camera using focal length 30 and depth 2024... \n");
	projectionMatrix = glm::perspective(30.0f, (float)windowWidth / (float)windowHeight, 0.1f, 2024.0f);
	printf("Render scene is set up. \n");
	
}

inline double diffclock( clock_t clock1, clock_t clock2 ) 
{
	double diffticks = clock1 - clock2;
    double diffms    = diffticks / ( CLOCKS_PER_SEC / 1000 );

    return diffms;
}

/**
 *	The Draw method
 */
void Render::Draw(GridModel* model, KinectTool* tool, glm::mat4& view, glm::mat4& obj )
{
	clock_t start = clock();
	
	glDepthMask(GL_TRUE);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	// Clear Screen And Depth Buffer
	glEnable(GL_CULL_FACE);
	glDisable(GL_BLEND);

	int i = 0;
	shader->bind();
	glUniformMatrix4fv(pMatrixLocation, 1, GL_FALSE, &(projectionMatrix[0][0]));
	glUniformMatrix4fv(mMatrixLocation, 1, GL_FALSE, &(obj[0][0]));
	glUniformMatrix4fv(vMatrixLocation, 1, GL_FALSE, &(view[0][0]));
	
	std::map< unsigned int, VAO* >* cells = model->GetRenderableCells();
	std::map< unsigned int, VAO* >::iterator iter;
	VAO* vao_ptr = NULL;

	for (iter = cells->begin(); iter != cells->end(); iter++)
	{
		vao_ptr = iter->second;
		glBindVertexArray(vao_ptr->id());
		 
		glDrawElements(
			 GL_POINTS,				// mode
			 vao_ptr->size(),		// count
			 GL_UNSIGNED_INT,		// type
			 (void*)0				// element array buffer offset
		 );
		i++;
	}
	
	glBindVertexArray(0);
	shader->unbind();
	
	glm::mat4 pvm = projectionMatrix*view;
	
	glDisable(GL_CULL_FACE);
	glEnable(GL_BLEND);	
	glDepthMask(GL_FALSE);
	vao_ptr = tool->GetToolMesh()->GetVAO();

	tool->GetToolShader()->bind();	
	glUniformMatrix4fv(tool->GetPVMLocation(), 1, GL_FALSE, &(pvm[0][0]));
	glBindVertexArray(vao_ptr->id());
	glDrawElements(
			GL_TRIANGLES,		// mode
			vao_ptr->size(),	// count
			GL_UNSIGNED_INT,	// type
			(void*)0			// element array buffer offset
		 );	
	glBindVertexArray(0);
	tool->GetToolShader()->unbind();
	
	////////////////////////////////////////////////////////////////////////////////////
	glLoadIdentity();									// Reset The Current Modelview Matrix
	text->glPrint("Hello World");
	printf("Hello World");
	//////////////////////////////////////////////////////////////////////////////////////
	///
	clock_t end = clock();
	//std::cout<<"For ticks = "<<i<<", tick time = " << diffclock( end, start )<< " ms" << std::endl;
}

/**
 *	 Resizes the camera/projectionmatrix
 */
void Render::Resize(int w, int h)
{
	printf("Resizes the projecton matrix... \n");
	glViewport(0, 0, w,  h);
	windowWidth = w;
	windowHeight = h;
	projectionMatrix = glm::perspective(30.0f, (float)windowWidth / (float)windowHeight, 0.1f, 2024.f);
}

