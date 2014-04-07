#ifndef TRIANGLE_MESH_H
#define TRIANGLE_MESH_H

#include "main.h"
#include "VBO.h"
#include "VAO.h"
#include "StereoKinectHeadTracking.h"
#include "NuiApi.h"

class TriangleMesh
{
public:
	TriangleMesh( unsigned int x, unsigned int y, float _start_x, float start_y, float end_x, float end_y, float start_z );
	~TriangleMesh();
	void UpdateDepth( float* _depth_map );
	void UpdateDepth( StereoKinectHeadTracking* tracking, vector<NUI_DEPTH_IMAGE_POINT>& points );
	VAO* GetVAO();
	Point* GetPoints();
private:
	void MakeVBO();
	inline void UpdateVBO();
	TriangleMesh();
	Point* _mesh;
	VAO* _vao;
	VBO* _vbo;
	unsigned int* _indexes;
	unsigned int _x, _y;
	unsigned int _index_cnt;
	float _start_z;
};

#endif // TRIANGLE_MESH_H

