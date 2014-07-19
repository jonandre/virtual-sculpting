#ifndef VAO_H
#define VAO_H
#include "VBO.h"
#include "GraphicsLib.h"
#include "main.h"

class VAO
{
public:
	VAO();
	~VAO();
	unsigned int id();
	unsigned int size();
	void bind ( const VBO& );
	
private:
	unsigned int vaoID[1];
	unsigned int sz;
	unsigned int counter;
};

#endif // VAO_H