#include "VAO.h"

/**
 * Creats a new Vertex Array Output 
 * Generats a open gl Vertex array 
 * 
 */
VAO::VAO( )
{
	sz = 0;
	// glGenVertexArrays returns n vertex array object names in arrays. 
	// There is no guarantee that the names form a contiguous set of integers;
	// however, it is guaranteed that none of the returned names was in use 
	// immediately before the call to glGenVertexArrays.
	glGenVertexArrays(1, &vaoID[0]);
	counter = 0;
}

unsigned int VAO::id()
{
	return vaoID[0];
}

void VAO::bind(const VBO& vbo)
{
	sz = vbo.size();
	// glBindVertexArray binds the vertex array object with name array
	// vaoID[0] is array name
	glBindVertexArray(vaoID[0]);	
	vbo.bind(counter);
	++counter;
	glBindVertexArray(0);
}

unsigned int VAO::size()
{
	return sz;
}

VAO::~VAO( )
{	
	glDeleteVertexArrays(1, &vaoID[0]);	
}