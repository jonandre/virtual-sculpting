#include "VBO.h"
#include "GraphicsLib.h"

#include "main.h"
VBO::VBO( const Point* vrt, const Color* clr, unsigned int* indices, unsigned int len, unsigned int indices_len, float* uv, const Normal* nrm )
{
	sz = len;

	vbo_cnt = 1;

	vbo_cnt += clr ? 1 : 0;
	vbo_cnt += uv ? 1 : 0;
	vbo_cnt += indices ? 1 : 0;
	vbo_cnt += nrm ? 1 : 0;

	glGenBuffers(1, &vboID[0]);

	glBindBuffer(GL_ARRAY_BUFFER, vboID[0]);
	glBufferData(GL_ARRAY_BUFFER, len*sizeof(Point), vrt, GL_STATIC_DRAW);

	if ( clr )
	{
		glGenBuffers(1, &vboID[1]);
		glBindBuffer(GL_ARRAY_BUFFER, vboID[1]);
		glBufferData(GL_ARRAY_BUFFER, len*sizeof(Color), clr, GL_STATIC_DRAW);  
	}
	else
		vboID[1] = 0;
	
	if ( uv )
	{
		glGenBuffers(1, &vboID[2]);
		glBindBuffer(GL_ARRAY_BUFFER, vboID[2]);
		glBufferData(GL_ARRAY_BUFFER, len*2*sizeof(float), uv, GL_STATIC_DRAW); 
	}
	else
		vboID[2] = 0;

	if ( nrm )
	{
		glGenBuffers(1, &vboID[4]);
		glBindBuffer(GL_ARRAY_BUFFER, vboID[4]);
		glBufferData(GL_ARRAY_BUFFER, len*sizeof(Normal), nrm, GL_STATIC_DRAW) ;
	}
	else
		vboID[4] = 0;

	if ( indices )
	{
		glGenBuffers(1, &vboID[3]);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboID[3]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices_len * sizeof(unsigned int), indices, GL_STATIC_DRAW);
		sz = indices_len;
	}
	else
		vboID[3] = 0;
}

void VBO::UpdateColorArray(const Color* clr, unsigned int len)
{
	if ( vboID[1] )
	{
		glDeleteBuffers(1, &vboID[1]);
	}

	glGenBuffers(1, &vboID[1]);
	glBindBuffer(GL_ARRAY_BUFFER, vboID[1]);
	glBufferData(GL_ARRAY_BUFFER, len*sizeof(Color), clr, GL_STATIC_DRAW); 
}


void VBO::UpdateIndexArray(unsigned int* indices, unsigned int len)
{
	if (vboID[3])
	{
		glDeleteBuffers(1, &vboID[3]);
	}

	glGenBuffers(1, &vboID[3]);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboID[3]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, len * sizeof(unsigned int), indices, GL_STATIC_DRAW);
	sz = len;
}


void VBO::UpdateVertexArray(const Point* vrt, unsigned int len)
{
	glDeleteBuffers(1, &vboID[0]);
	glGenBuffers(1, &vboID[0]);

	glBindBuffer(GL_ARRAY_BUFFER, vboID[0]);
	glBufferData(GL_ARRAY_BUFFER, len*sizeof(Point), vrt, GL_STATIC_DRAW);
}

const unsigned int VBO::bind(unsigned int start) const
{

	glBindBuffer(GL_ARRAY_BUFFER, vboID[0]);
	glVertexAttribPointer((GLuint)start, sizeof(Point)/sizeof(float), GL_FLOAT, GL_FALSE, sizeof(Point), 0);
	glEnableVertexAttribArray(start);
	
		
	if ( vboID[1] )
	{
		glBindBuffer(GL_ARRAY_BUFFER, vboID[1]);		
		glVertexAttribPointer((GLuint)1, sizeof(Color)/sizeof(UINT8), GL_UNSIGNED_BYTE, GL_FALSE, sizeof(Color), 0);
		glEnableVertexAttribArray(1);
	}
	if ( vboID[2] )
	{
		glBindBuffer(GL_ARRAY_BUFFER, vboID[2]);
		glVertexAttribPointer((GLuint)2, 2, GL_FLOAT, GL_FALSE, 2*sizeof(float), 0);
		glEnableVertexAttribArray(2);	 
	}
	if ( vboID[4] )
	{
		glBindBuffer(GL_ARRAY_BUFFER, vboID[4]);
		glVertexAttribPointer((GLuint)3, sizeof(Normal)/sizeof(float), GL_FLOAT, GL_FALSE, sizeof(Normal), 0);
		glEnableVertexAttribArray(3);	 
	}
	if (vboID[3])
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboID[3]);
	}


	glBindBuffer(GL_ARRAY_BUFFER, 0);
	return vbo_cnt;
}


unsigned int VBO::size() const
{
	return sz;
}

VBO::~VBO()
{
	glDeleteBuffers(1, &vboID[0]);
	if ( vboID[1] )
	{
		glDeleteBuffers(1, &vboID[1]);
	}
	if ( vboID[2] )
	{
		glDeleteBuffers(1, &vboID[2]);
	}
	if (vboID[3])
	{
		glDeleteBuffers(1, &vboID[3]);
	}
	if (vboID[4])
	{
		glDeleteBuffers(1, &vboID[4]);
	}
}