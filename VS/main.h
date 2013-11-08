#pragma once
#include <string>
#include <iostream>
#include <vector>

#include <fstream>

using namespace std;

#include "glm\glm.hpp"  
#include "glm\gtc\matrix_transform.hpp"
#include "glm\gtx\orthonormalize.hpp"
#include "glm\gtx\quaternion.hpp"
#include "glm\gtc\matrix_transform.hpp"
#include "glm\gtc\quaternion.hpp"

typedef unsigned char UINT8;

struct Color
{
	UINT8 comp[4];
};

struct Point
{
	float coord[3];
};

struct Normal
{
	float comp[3];
};

struct Vertex
{
	Point _point;//3*4
	Normal _normal;//3*4
	Color _color;//4*1
	float _uv[2];//2*4
	//Totall = 12+12+4+8 bytes == 36 - we need padding
	UINT8 _padding[28];
};


static string textFileRead(const char *fileName) 
{
	string fileString;
	string line;
	
	ifstream file(fileName,ios_base::in);

	if (file.is_open()) 
	{
		while (!file.eof()) 
		{
			getline(file, line);
		  	fileString.append(line);
			fileString.append("\n");
		}
		file.close();
	}
	else
		cout<<"Unable to open "<<fileName<<"\n";

    return fileString;
}