#include "STLExporter.h"
#include <time.h>
#include <iomanip>
#include <string>
#include <sstream>
#include <algorithm>

inline int STLExporter::ref (int dimm, int i, int j, int k) {
	return i*dimm*dimm + j*dimm + k;
}

inline int STLExporter::dirRef (int dimm, direction dir, int i, int j, int k) {
	switch (dir) {
	case up:
		if (j + 1 == dimm) return -1;
		return ref(dimm, i, j+1, k);
		break;
	case down:
		if (j == 0) return -1;
		return ref(dimm, i, j-1, k);
		break;
	case right:
		if (i + 1 == dimm) return -1;
		return ref(dimm, i + 1, j, k);
		break;
	case left:
		if (i == 0) return -1;
		return ref(dimm, i - 1, j, k);
		break;
	case forward:
		if (k + 1 == dimm) return -1;
		return ref(dimm, i, j, k + 1);
		break;
	case backward:
		if (k == 0) return -1;
		return ref(dimm, i, j, k - 1);
		break;
	default:
		return -1;
	}
}

inline glm::vec3 STLExporter::dirVector (direction normal, direction dir) {
	glm::vec3 upv (0.0f, 1.0f, 0.0f);
	glm::vec3 rightv (1.0f, 0.0f, 0.0f);
	glm::vec3 forwardv (0.0f, 0.0f, 1.0f);

	switch (normal) {
	case up:
		switch (dir) {
		case up: return upv;
		case right: return rightv;
		case forward: return forwardv;
		default: return upv;
		};
	case down:
		switch (dir) {
		case up: return -upv;
		case right: return rightv;
		case forward: return -forwardv;
		default: return -upv;
		};
	case right:
		switch (dir) {
		case up: return rightv;
		case right: return -upv;
		case forward: return forwardv;
		default: return rightv;
		};
	case left:
		switch (dir) {
		case up: return -rightv;
		case right: return upv;
		case forward: return forwardv;
		default: return -rightv;
		};
	case forward:
		switch (dir) {
		case up: return forwardv;
		case right: return rightv;
		case forward: return -upv;
		default: return forwardv;
		};
	case backward:
		switch (dir) {
		case up: return -forwardv;
		case right: return -rightv;
		case forward: return -upv;
		default: return -forwardv;
		};
	default:
		return glm::vec3(0.0f);
	};
}

// The base point of the quad has to change depending wich face we are directing
glm::vec3 STLExporter::basePoint (glm::vec3 center, direction dir)
{
	glm::vec3 upv (0.0f, 1.0f, 0.0f);
	glm::vec3 rightv (1.0f, 0.0f, 0.0f);
	glm::vec3 forwardv (0.0f, 0.0f, 1.0f);

	switch (dir) {
	case up:
		return center + upv;
	case down:
		return center + forwardv;
	case right:
		return center + upv + rightv;
	case left:
		return center;
	case forward:
		return center + upv + forwardv;
	case backward:
		return center + rightv + upv;
	default:
		return center;
	}
}

void STLExporter::ExportToStl(unsigned char* voxels, int dimm)
{
	std::ofstream outFile;
	time_t rawTime;
	struct tm * timeInfo;

	time (&rawTime);
	timeInfo = localtime(&rawTime);
	string timeString(asctime(timeInfo));
	timeString.pop_back();
	replace(timeString.begin(), timeString.end(), ' ', '_');
	replace(timeString.begin(), timeString.end(), ':', '-'); 

	std::stringstream ss;
	ss << "Milo_sculpture_" << timeString << ".stl";

	std::cout << "Saving file " << ss.str() << std::endl;

	outFile.open(ss.str(), std::ofstream::out | std::ofstream::trunc);

	if (!outFile) {
		std::cout << std::strerror(errno) << std::endl;
	}

	outFile << "solid milosculpture" << std::endl;
	outFile << std::scientific;

	for (int i = 0; i < dimm; ++i) {
		for (int j = 0; j < dimm; ++j) {
			for (int k = 0; k < dimm; ++k) {
				for (int d = direction::up; d < direction::length; ++d) {

					// if the voxel is empty
					if (voxels[ref(dimm,i,j,k)] <= 0) continue; 

					int check = dirRef(dimm, (direction)d, i, j, k);

					// if there is nothing on the direction
					if (check < 0 || voxels[check] <= 0) {
						glm::vec3 normal = dirVector((direction)d, (direction)up);
						glm::vec3 rightv = dirVector((direction)d, (direction)right);
						glm::vec3 forwardv = dirVector((direction)d, (direction)forward);

						glm::vec3 center = glm::vec3(float(i), float(j), float(k));
						center = basePoint(center, (direction)d);
						// Vertex representation
						// .--.
						// |__|

						glm::vec3 v0 = center; // |_
						glm::vec3 v1 = center + forwardv; // .-
						glm::vec3 v2 = center + forwardv + rightv; // -.
						glm::vec3 v3 = center + rightv; // _|

						outFile << "facet normal " << normal.x << " " << normal.y << " " << normal.z << std::endl;
						outFile << "outer loop" << std::endl;
						outFile << "vertex " << v0.x << " " << v0.y << " " << v0.z << std::endl;
						outFile << "vertex " << v1.x << " " << v1.y << " " << v1.z << std::endl;
						outFile << "vertex " << v2.x << " " << v2.y << " " << v2.z << std::endl;
						outFile << "endloop" << std::endl;
						outFile << "endfacet" << std::endl;

						outFile << "facet normal " << normal.x << " " << normal.y << " " << normal.z << std::endl;
						outFile << "outer loop" << std::endl;
						outFile << "vertex " << v0.x << " " << v0.y << " " << v0.z << std::endl;
						outFile << "vertex " << v2.x << " " << v2.y << " " << v2.z << std::endl;
						outFile << "vertex " << v3.x << " " << v3.y << " " << v3.z << std::endl;
						outFile << "endloop" << std::endl;
						outFile << "endfacet" << std::endl;
					}
				}
			}
		}
	}

	outFile << "endsolid milosculpture" << std::endl;

	outFile.close();
}