#include "TriangleMesh.h"
#include "main.h"
#include "VBO.h"
#include "VAO.h"


TriangleMesh::TriangleMesh()
{
}

TriangleMesh::TriangleMesh(unsigned int xx, unsigned int yy, float start_x, float start_y, float end_x, float end_y, float start_z)
{
	float x_step = (end_x - start_x) / (float)(xx);
	float y_step = (end_y - start_y) / (float)(yy);
	unsigned int numIndices = 0;
	unsigned int xn = (xx - 1) * yy, yn = yy - 1, x, y;
	
	_mesh = new Point[xx * yy];
	_vao = NULL;
	_vbo = NULL;
	_x = xx;
	_y = yy;
	_start_z = start_z;
	
	_index_cnt = (xx - 1) * (yy - 1) * 6;
	this->indices = new unsigned int[_index_cnt];
	
	for (x = 0; x < xx; x++)
		for (y = 0; y < yy; y++)
		{
			_mesh[x * _y + y].coord[0] = start_x + x * x_step;
			_mesh[x * _y + y].coord[1] = start_y + y * y_step;
			_mesh[x * _y + y].coord[2] = start_z; /* - (rand() % 30) / 1.0f */;
		}
	
	for (x = 0; x < xn; x += yy)
		for (y = 0; y < yn; y++)
		{
			unsigned int a = x      + y;
			unsigned int b = x + yy + y;
			
			this->indices[numIndices + 0] = a + 1;
			this->indices[numIndices + 1] = b;
			this->indices[numIndices + 2] = a;
			
			this->indices[numIndices + 3] = a;
			this->indices[numIndices + 4] = a + 1;
			this->indices[numIndices + 5] = b + 1;
			
			numIndices += 6;
		}
	
	MakeVBO();
}


inline void TriangleMesh::UpdateVBO()
{
	_vbo->UpdateVertexArray(_mesh, _x * _y);
	
	if (_vao)  delete _vao;
	_vao = new VAO();
	_vao->bind(*_vbo);
}


void TriangleMesh::UpdateDepth(float* _depth_map)
{
	unsigned int x, y;
	
	for (x = 0; x < _x; x++)
		for (y = 0; y < _y; y++)
		{
			/* _mesh[x * _y + y].coord[2] = _start_z + ((_depth_map[(_y - y) * _x + x] + _depth_map[(_y - y - 1) * _x + x - 1] + 
				_depth_map[(_y - y - 1) * _x + x + 1] + _depth_map[(_ y - y - 2) * _x + x]) / 4.0f + _depth_map[(_y - y - 1) * _x + x]) / 2.0f; */
			_mesh[x * _y + y].coord[2] = _start_z + _depth_map[(_y - y - 1) * _x + x];
		}
	
	//MakeVBO();
	UpdateVBO();
}

Point* TriangleMesh::GetPoints()
{
	return _mesh;
}

void TriangleMesh::MakeVBO()
{
	if (_vbo)  delete _vbo;
	_vbo = new VBO(_mesh, 0, this->indices, _x * _y, _index_cnt);
	
	if (_vao)  delete _vao;
	_vao = new VAO();
	_vao->bind(*_vbo);
}

VAO* TriangleMesh::GetVAO()
{
	return _vao;
}


TriangleMesh::~TriangleMesh()
{
	delete [] _mesh;
	delete [] this->indices;
	if (_vbo)  delete _vbo;
	if (_vao)  delete _vao;
}

