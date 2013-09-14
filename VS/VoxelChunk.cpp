#include "VoxelChunk.h"
#include "VBO.h"
#include "VAO.h"
#include "GraphicsLib.h"


//Point* VoxelChunk::_points = NULL;
//Normal* VoxelChunk::_normals = NULL;

VoxelChunk::VoxelChunk()
{
}


void VoxelChunk::CreateGeometry()
{
	unsigned int global_size = pow3(size);
	//Each visual element is point
	_points = new Point[global_size]; //currently we are not counting shared points between voxels
	_colors = new Color[global_size];
	memset(_colors, 0, global_size * sizeof(Color));
	_renderable_indexes = new unsigned int[global_size];
	_vertex_len = global_size;
	_indexes = new unsigned int[global_size];
	
	//now we can generate geometry in given chunk - later it can be updated(color arrays, for example)
	
	Color __clr;
	__clr.comp[0] = 255;
	__clr.comp[1] = 255;
	__clr.comp[2] = 255;
	__clr.comp[3] = 0;
	
	Point pnt;
	unsigned int index = 0;
	
	for (unsigned int i = 0; i < size; i++)
	{
		pnt.coord[0] = (float)(_lbl[0] + i) + 0.5f;
		for (unsigned int j = 0; j < size; j++)
		{
			pnt.coord[1] = (float)(_lbl[1] + j) + 0.5f;
			for (unsigned int k = 0; k < size; k++)
			{
				pnt.coord[2] = (float)(_lbl[2] + k) + 0.5f;
				index = poly3(i, j, k, size);
				_points[index] = pnt;
				_colors[index] = __clr;
			}
		}
	}
}

VoxelChunk::VoxelChunk(const Point& center, int lbl[3], int ufr[3])
{
	_center = center;
	_vao = NULL;
	_vbo = NULL;
	_dirty = false;
	size = ufr[0] - lbl[0];
	memcpy(_lbl, lbl, 3 * sizeof(int));
	memcpy(_ufr, ufr, 3 * sizeof(int));
	_points = NULL;
	_colors = NULL;
	_vertex_len = 0;
	_indexes = NULL;
	_renderable_indexes = NULL;
}


VoxelChunk::~VoxelChunk()
{	
	ClearGeometry();
	ClearMesh();
}

/*
static void polygon(int a, int b, int c , int d, Point* vert, Color* clr, GLfloat vertices[][3])
{
	vert[0].coord[0] = vertices[a][0];
	vert[0].coord[1] = vertices[a][1];
	vert[0].coord[2] = vertices[a][2];
	
	vert[1].coord[0] = vertices[b][0];
	vert[1].coord[1] = vertices[b][1];
	vert[1].coord[2] = vertices[b][2];
	
	vert[2].coord[0] = vertices[c][0];
	vert[2].coord[1] = vertices[c][1];
	vert[2].coord[2] = vertices[c][2];
	
	vert[3].coord[0] = vertices[c][0];
	vert[3].coord[1] = vertices[c][1];
	vert[3].coord[2] = vertices[c][2];
	
	vert[4].coord[0] = vertices[d][0];
	vert[4].coord[1] = vertices[d][1];
	vert[4].coord[2] = vertices[d][2];
	
	vert[5].coord[0] = vertices[a][0];
	vert[5].coord[1] = vertices[a][1];
	vert[5].coord[2] = vertices[a][2];
	
	
	for (int i = 0; i < 6; i++)
		clr[i] = *clr;
}
*/

void VoxelChunk::ClearMesh()
{
	if (_vbo)
		delete _vbo;
	_vbo = NULL;
	
	if (_vao)
		delete _vao;
	_vao = NULL;
}

inline unsigned char VoxelChunk::EvaluateCell(unsigned char* m_pBlocks, unsigned int x, unsigned int y, unsigned int z, unsigned int dimm) //check neighbours, basically "is visible" for given cell
{
	unsigned int index = poly3(x, y, z, dimm);
	
	if (m_pBlocks[index] == 0)
		return 0;
	
	unsigned char res = 0; //Coding 00xxxxxx - where x is side
	
	//top
	if ((y == dimm - 1) || m_pBlocks[index + dimm] == 0)
		++res;
	res <<= 1;
	
	//bottom
	if ((y == 0) || m_pBlocks[index - dimm] == 0)
		 ++res;
	res <<= 1;
	
	//right
	if ((x == dimm - 1) || m_pBlocks[index + pow2(dimm)] == 0)
		++res;
	res <<= 1;
	
	//left
	if ((x == 0) || m_pBlocks[index - pow2(dimm)] == 0)
		++res;
	res <<= 1;
	
	//back
	if ((z == 0) || m_pBlocks[index - 1] == 0)
		++res;
	res <<= 1;
	
	//front
	if ((z == dimm - 1) || m_pBlocks[index + 1] == 0)
		++res;
	
	//if (res == 63) //if cell has no neighbours - remove it. Hack, anyway.
	//{
		//m_pBlocks[index] = 0;
		//return 0;
	//}
	
	return res;
}

unsigned char VoxelChunk::GetVoxelAlpha(unsigned int x, unsigned int y, unsigned int z)
{
	return _colors ? _colors[poly3(x, y, z, size)].comp[3] : 0;
}

inline void VoxelChunk::SetColorForVoxel(unsigned int x, unsigned int y, unsigned int z, Color* clr)
{
	_colors[poly3(x, y, z, size)] = *clr;
}

void VoxelChunk::ClearGeometry()
{
	if (_points)              delete [] _points;
	if (_colors)              delete [] _colors;
	if (_indexes)             delete [] _indexes;
	if (_renderable_indexes)  delete [] _renderable_indexes;
	
	_points = NULL;
	_colors = NULL;
	_vertex_len = 0;
	_indexes = NULL;
	_renderable_indexes = NULL;
}

inline void MapColor(Color* clr, unsigned char val, bool acted)
{
	(void) val; /* TODO: why does val exist? */
	
	if (acted)
	{
		clr->comp[0] = 255;
		clr->comp[1] = 0;
		clr->comp[2] = 0;
	}
	else
	{
		clr->comp[0] = 0;
		clr->comp[1] = 250;
		clr->comp[2] = 250;
	}
}

void VoxelChunk::CreateMesh(unsigned char* m_pBlocks, bool* _acted, unsigned int dimm) //lower corner
{
	if (!_vao)
		_vao = new VAO();
	
	_dirty = false;
	
	Color clr;
	clr.comp[0] = 255;
	clr.comp[1] = 255;
	clr.comp[2] = 255;
	clr.comp[3] = 255;
	int h_dimm = dimm >> 1;
	const int _local_to_global_i = h_dimm + _lbl[0];
	const int _local_to_global_j = h_dimm + _lbl[1];
	const int _local_to_global_k = h_dimm + _lbl[2];
	
	unsigned int num_created = 0;
	unsigned int global_index = 0;
	unsigned char tmp_res = 0;
	unsigned int x, y;
	
	#define __(I) (_local_to_global_##I + I)
	for (unsigned int i = 0; i < size; i++)
	{
		x = __(i) * dimm * dimm;
		for (unsigned int j = 0; j < size; j++)
		{
			y = __(j) * dimm;
			for (unsigned int k = 0; k < size; k++)
			{
				tmp_res = EvaluateCell(m_pBlocks, __(i), __(j), __(k), dimm);
				if (tmp_res)
				{
					if (_points == NULL)
						CreateGeometry();
					
					if (!_vbo)
						_vbo = new VBO(_points, NULL, 0, _vertex_len);
					
					global_index = (x + y + __(k));
					//MapColor(&clr, m_pBlocks[global_index], false);
					MapColor(&clr, m_pBlocks[global_index], _acted[global_index]);
					
					_acted[global_index] = false;
					clr.comp[3] = tmp_res;
					SetColorForVoxel(i, j, k, &clr);
					_renderable_indexes[num_created] = poly3(i, j, k, size);
					++num_created;
				}
			}
		}
	}
	#undef __
	
	_renderable_indexes_count = num_created;
	
	if (num_created)
	{
		_vbo->UpdateColorArray(_colors, _vertex_len);
		_vbo->UpdateIndexArray(_renderable_indexes, _renderable_indexes_count);
		_vao->bind(*_vbo);
	}
	else
	{
		if (_points != NULL)
			ClearGeometry();
		ClearMesh();
	}
}

void VoxelChunk::RecalcColor(unsigned char* voxels, unsigned int dimm)
{
	if (!_vao)
		return;
	
	Color clr;
	int h_dimm = dimm >> 1;
	const int _local_to_global_i = h_dimm + _lbl[0];
	const int _local_to_global_j = h_dimm + _lbl[1];
	const int _local_to_global_k = h_dimm + _lbl[2];
	
	unsigned int x, y, index;
	
	#define __(I) (_local_to_global_##I + I)
	for (unsigned int i = 0; i < size; i++)
	{
		x = __(i) * dimm * dimm;
		for (unsigned int j = 0; j < size; j++)
		{
			y = __(j) * dimm;
			for (unsigned int k = 0; k < size; k++)
			{
				MapColor(&clr, voxels[x + y + __(k)], false);
				index = poly3(i, j, k, size);
				_colors[index].comp[0] = clr.comp[0];
				_colors[index].comp[1] = clr.comp[1];
				_colors[index].comp[2] = clr.comp[2];
			}
		}
	}
	#undef __
	
	_vbo->UpdateColorArray(_colors, _vertex_len);
	_vao->bind(*_vbo);
}


VAO* VoxelChunk::GetVAO()
{
	return _vao;
}

const Point& VoxelChunk::GetCenter()
{
	return _center;
}

void VoxelChunk::MarkDirty()
{
	_dirty = true;
}

bool VoxelChunk::IsDirty()
{
	return _dirty;
}

void VoxelChunk::UpdateGeometry()
{

}

