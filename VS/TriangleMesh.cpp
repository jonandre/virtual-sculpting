#include "TriangleMesh.h"
#include "main.h"
#include "VBO.h"
#include "VAO.h"

TriangleMesh::TriangleMesh()
{
}

TriangleMesh::TriangleMesh( unsigned int xx, unsigned int yy, float start_x, float start_y, float end_x, float end_y, float start_z )
{
	_mesh = new Point[xx*yy];
	_vao = NULL;
	_vbo = NULL;
	_x = xx;
	_y = yy;
	_start_z = start_z;

	_index_cnt = (_x-1)*(_y-1)*6;
	_indexes = new unsigned int[_index_cnt];

	float x_step = (end_x - start_x)/float(xx);
	float y_step = (end_y - start_y)/float(yy);

	for ( unsigned int i = 0; i < _x; i++ )
	{
		for ( unsigned int j = 0; j < _y; j++ )
		{
			_mesh[ i*_y + j ].coord[0] = start_x + i*x_step;
			_mesh[ i*_y + j ].coord[1] = start_y + j*y_step;
			_mesh[ i*_y + j ].coord[2] = _start_z;// - (rand()%30)/1.0f;
		}
	}
	unsigned int numIndices = 0;

	for (int x = 0; x < _x -1; ++x ) 
	{
		for (int z = 0; z < _y -1; ++z ) 
		{
			int a = ( x * ( _y )) + z ;
			int b = (( x +1) * ( _y )) + z ;
			int c = (( x +1) * ( _y )) + ( z +1);
			int d = ( x * ( _y )) + ( z +1);

			_indexes [ numIndices ++] = c ;
			_indexes [ numIndices ++] = b ;
			_indexes [ numIndices ++] = a ;

			_indexes [ numIndices ++] = a ;
			_indexes [ numIndices ++] = d ;
			_indexes [ numIndices ++] = c ;
		}
	}
	MakeVBO();
}


inline void TriangleMesh::UpdateVBO()
{
	_vbo->UpdateVertexArray(_mesh, _x*_y );
	if ( _vao )
		delete _vao;

	_vao = new VAO();
	_vao->bind(*_vbo);
}


void TriangleMesh::UpdateDepth( float* _depth_map )
{
	for ( unsigned int i = 0; i < _x; i++ )
	{
		for ( unsigned int j = 0; j < _y; j++ )
		{
			/*_mesh[ i*_y + j ].coord[2] = _start_z + ((_depth_map[ (_y-j)*_x + i ] + _depth_map[ (_y-j-1)*_x + i -1] + 
				_depth_map[ (_y-j-1)*_x + i + 1 ] + _depth_map[ (_y-j-2)*_x + i ])/4.0f + _depth_map[ (_y-j-1)*_x + i ])/2.0f;*/
			_mesh[ i*_y + j ].coord[2] = _start_z + _depth_map[ (_y-j-1)*_x + i ];
		}
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
	if ( _vbo )
		delete _vbo;
	_vbo = new VBO( _mesh, 0,_indexes, _x*_y, _index_cnt );

	if ( _vao )
		delete _vao;

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
	delete [] _indexes;
	if ( _vbo )
		delete _vbo;

	if ( _vao )
		delete _vao;
}
