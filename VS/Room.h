#ifndef ROOM_H
#define ROOM_H

#include "Shader.h"
#include "main.h"

class Room
{
public:
	
	Room();
	~Room();

	void Draw();
private:
	Shader* shader;

};

#endif