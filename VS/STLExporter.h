#ifndef STL_EXPORTER_H
#define STL_EXPORTER_H

#include "main.h"
#include <iostream>


class STLExporter {
private:
	enum direction {
		up = 0,
		down = 1,
		left = 2,
		right = 3,
		forward = 4,
		backward= 5,
		length = 6
	};

	static int ref (int dimm, int i, int j, int k);
	static int dirRef (int dimm, direction dir, int i, int j, int k);
	static glm::vec3 dirVector (direction normal, direction dir);
	static glm::vec3 basePoint (glm::vec3 center, direction dir);

public:
	static void ExportToStl (string sesionTime, unsigned char* voxels, int dimm);
};

#endif