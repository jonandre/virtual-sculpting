/*
This calss is wrapper for visual representation of some block of our voxelgrid. Idea - split whole 3D array into chunks:
128^3 is splited in blocks of 32, for example. This is important in order to reduce amount of drawcalls - 4^3 is MUCH less than 128^3. But beware,
it's a tradeoff - larger block is, more expencive update is.
*/
#pragma once

#include "main.h"
class VAO;
class VBO;

class VoxelChunk
{
public:
	VoxelChunk();
	VoxelChunk( const Point& center, int lbl[3], int ufr[3] );//lbl (lower back left) and ufr - 
											//actual int aabb coords - used to generate initial geometry
	~VoxelChunk();

	void Update(float dt);
    void Render();
	void CreateMesh( UINT8* voxels, bool* _acted, unsigned int dimm );
	VAO* GetVAO();
	const Point& GetCenter();
	bool IsDirty();
	void MarkDirty();
	UINT8 GetVoxelAlpha( unsigned int x, unsigned int y, unsigned int z ); // In alpha chanel we have
	void RecalcColor( UINT8* voxels, unsigned int dim );

private:
	void ClearMesh();
	inline UINT8 EvaluateCell(  UINT8* m_pBlocks, unsigned int x, unsigned int y, unsigned int z, unsigned int dimm );//check neighbours, basically "is visible" for given cell

	inline void SetColorForVoxel( unsigned int x, unsigned int y, unsigned int z, Color* clr );
	void CreateGeometry();
	void ClearGeometry();
	void UpdateGeometry();
    // The blocks data
    //VoxelBlock*** m_pBlocks;
	VAO* _vao;
	//std::vector<VBO*> _vbos;
	VBO* _vbo;
	Point _center;
	int _lbl[3];
	int _ufr[3];

	Point* _points;//idea - we have fast access to chang some part of data. Visibility is defined by inices
	Color* _colors;

	unsigned int _vertex_len;
	unsigned int* _indexes;
	unsigned int* _renderable_indexes;
	unsigned int _renderable_indexes_count;
	unsigned int size;
	bool _dirty;

};

