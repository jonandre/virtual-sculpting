#include "TriangleMesh.h"

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
			_mesh[ i*_y + j ].coord[2] = _start_z + _depth_map[ (_y-j-1)*_x + i ];
		}
	}
	UpdateVBO();
}

void TriangleMesh::UpdateDepth( StereoKinectHeadTracking* tracking, vector<NUI_DEPTH_IMAGE_POINT>& points )
{
	INuiCoordinateMapper* coordMapper;
	tracking->GetSensor()->NuiGetCoordinateMapper(&coordMapper);

	for ( unsigned int i = 0; i < _x; i++ )
	{
		for ( unsigned int j = 0; j < _y; j++ )
		{
			NUI_DEPTH_IMAGE_POINT o = points[(_y-j-1)*_x + i];
			Vector4 p;

			coordMapper->MapDepthPointToSkeletonPoint(NUI_IMAGE_RESOLUTION_640x480, &points[(_y-j-1)*_x + i], &p);
			//p = NuiTransformDepthImageToSkeleton(o.x, o.y, o.depth, NUI_IMAGE_RESOLUTION_640x480);
			
			glm::vec3 rwPoint = glm::vec3(p.x, p.y, p.z);
			glm::vec3 vwPoint = tracking->SensorToVirtualWorldCoordinates(rwPoint);
			vwPoint += glm::vec3(-0.027f, 0.03f, 0.013f); // Seen error

			if (false/*i == _x/2 && j == 0*/) {
				std::cout << "Depth conversion " << i <<","<<j<<":" << std::endl;
				std::cout << o.x << "," << o.y << "," << o.depth << std::endl;
				std::cout << "->" << p.x << "," << p.y << "," << p.z << std::endl;
				std::cout << "VW:" << vwPoint.x << "," << vwPoint.y << "," << vwPoint.z << endl;
				std::cout << "-------------------------" << std::endl;
			}
			_mesh[ i*_y + j ].coord[0] = vwPoint.x;
			_mesh[ i*_y + j ].coord[1] = vwPoint.y;
			_mesh[ i*_y + j ].coord[2] = vwPoint.z;
		}
	}
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
