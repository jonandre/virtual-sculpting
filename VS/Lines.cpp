#include "Lines.h"

#include <gl/glew.h>
#include <gl/freeglut.h>
#include <iostream>


Lines::Lines(void)
{
	printf("Initiating courdinate lines... \n");

	

	_lines_shader.LoadFromFile(GL_VERTEX_SHADER,"lines.vert");
	_lines_shader.LoadFromFile(GL_FRAGMENT_SHADER,"lines.frag");
	_lines_shader.CreateAndLinkProgram();


	_lines_shader.Use();	
		_lines_shader.AddAttribute("vVertex");
		_lines_shader.AddAttribute("vTexCoord");
	_lines_shader.UnUse();

}


Lines::~Lines(void)
{
}

void Lines::DrawLines(void)
{

	glBegin( GL_LINES );
	glVertex3f( 0.0, 0.0, 0.0 );
	glVertex3f( 10.0, 10.0, 10.0);
	glEnd();
}
