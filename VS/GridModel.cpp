#include "GridModel.h"
#include "VoxelChunk.h"
#include "VoxelBlock.h"
#include <pthread.h>

#define USE_SPINLOCK

#ifdef USE_SPINLOCK
static pthread_spinlock_t spinlock;
#else
static pthread_mutex_t mutex;
#endif


GridModel::GridModel()
{
	GridModel(128);
}

/// Code to generate nice rock.
float grad[12][3] = {
    {1.0,1.0,0.0},{-1.0,1.0,0.0},{1.0,-1.0,0.0},{-1.0,-1.0,0.0},
    {1.0,0.0,1.0},{-1.0,0.0,1.0},{1.0,0.0,-1.0},{-1.0,0.0,-1.0},
    {0.0,1.0,1.0},{0.0,-1.0,1.0},{0.0,1.0,-1.0},{0.0,-1.0,-1.0}
};

int perm[512] = {151, 160, 137, 91, 90, 15, 131, 13, 201, 95, 96, 53, 194, 233, 7, 225, 140, 36, 103, 30, 69, 142, 8, 99, 37, 240, 21, 10, 23, 190, 6, 148, 247, 120, 234, 75, 0, 26, 197, 62, 94, 252, 219, 203, 117, 35, 11, 32, 57, 177, 33, 88, 237, 149, 56, 87, 174, 20, 125, 136, 171, 168, 68, 175, 74, 165, 71, 134, 139, 48, 27, 166, 77, 146, 158, 231, 83, 111, 229, 122, 60, 211, 133, 230, 220, 105, 92, 41, 55, 46, 245, 40, 244, 102, 143, 54, 65, 25, 63, 161, 1, 216, 80, 73, 209, 76, 132, 187, 208, 89, 18, 169, 200, 196, 135, 130, 116, 188, 159, 86, 164, 100, 109, 198, 173, 186, 3, 64, 52, 217, 226, 250, 124, 123, 5, 202, 38, 147, 118, 126, 255, 82, 85, 212, 207, 206, 59, 227, 47, 16, 58, 17, 182, 189, 28, 42, 223, 183, 170, 213, 119, 248, 152, 2, 44, 154, 163, 70, 221, 153, 101, 155, 167, 43, 172, 9, 129, 22, 39, 253, 19, 98, 108, 110, 79, 113, 224, 232, 178, 185, 112, 104, 218, 246, 97, 228, 251, 34, 242, 193, 238, 210, 144, 12, 191, 179, 162, 241, 81, 51, 145, 235, 249, 14, 239, 107, 49, 192, 214, 31, 181, 199, 106, 157, 184, 84, 204, 176, 115, 121, 50, 45, 127, 4, 150, 254, 138, 236, 205, 93, 222, 114, 67, 29, 24, 72, 243, 141, 128, 195, 78, 66, 215, 61, 156, 180, 151, 160, 137, 91, 90, 15, 131, 13, 201, 95, 96, 53, 194, 233, 7, 225, 140, 36, 103, 30, 69, 142, 8, 99, 37, 240, 21, 10, 23, 190, 6, 148, 247, 120, 234, 75, 0, 26, 197, 62, 94, 252, 219, 203, 117, 35, 11, 32, 57, 177, 33, 88, 237, 149, 56, 87, 174, 20, 125, 136, 171, 168, 68, 175, 74, 165, 71, 134, 139, 48, 27, 166, 77, 146, 158, 231, 83, 111, 229, 122, 60, 211, 133, 230, 220, 105, 92, 41, 55, 46, 245, 40, 244, 102, 143, 54, 65, 25, 63, 161, 1, 216, 80, 73, 209, 76, 132, 187, 208, 89, 18, 169, 200, 196, 135, 130, 116, 188, 159, 86, 164, 100, 109, 198, 173, 186, 3, 64, 52, 217, 226, 250, 124, 123, 5, 202, 38, 147, 118, 126, 255, 82, 85, 212, 207, 206, 59, 227, 47, 16, 58, 17, 182, 189, 28, 42, 223, 183, 170, 213, 119, 248, 152, 2, 44, 154, 163, 70, 221, 153, 101, 155, 167, 43, 172, 9, 129, 22, 39, 253, 19, 98, 108, 110, 79, 113, 224, 232, 178, 185, 112, 104, 218, 246, 97, 228, 251, 34, 242, 193, 238, 210, 144, 12, 191, 179, 162, 241, 81, 51, 145, 235, 249, 14, 239, 107, 49, 192, 214, 31, 181, 199, 106, 157, 184, 84, 204, 176, 115, 121, 50, 45, 127, 4, 150, 254, 138, 236, 205, 93, 222, 114, 67, 29, 24, 72, 243, 141, 128, 195, 78, 66, 215, 61, 156, 180};

inline float dot(float x, float y, float z, float* g){
    return x*g[0] + y*g[1] + z*g[2];
}

inline float noise(float xin, float yin, float zin){
    float F3, G3, t, X0, Y0, Z0, x0, y0, z0, s, x1, y1, z1, x2, y2, z2, x3, y3, z3, t0, t1, t2, t3, n0, n1, n2, n3;
    int i, j, k, ii, jj, kk, i1, j1, k1, i2, j2, k2, gi0, gi1, gi2, gi3;
    
    F3 = 1.0f/3.0f;
    s = (xin+yin+zin)*F3;
    i = xin+s;
    j = yin+s;
    k = zin+s;
    G3 = 1.0/6.0;
    t = (i+j+k)*G3;
    X0 = i-t;
    Y0 = j-t;
    Z0 = k-t;
    x0 = xin-X0;
    y0 = yin-Y0;
    z0 = zin-Z0;
    
    if(x0 >= y0){
        if(y0 >= z0){
            i1=1; j1=0; k1=0; i2=1; j2=1; k2=0;
        }
        else if(x0 >= z0){
             i1=1; j1=0; k1=0; i2=1; j2=0; k2=1;
        }
        else{
            i1=0; j1=0; k1=1; i2=1; j2=0; k2=1;
        }
    }
    else{
        if(y0 < z0){
            i1=0; j1=0; k1=1; i2=0; j2=1; k2=1;
        }
        else if(x0 < z0){ 
            i1=0; j1=1; k1=0; i2=0; j2=1; k2=1;
        }
        else{
            i1=0; j1=1; k1=0; i2=1; j2=1; k2=0;
        }
    }

    x1 = x0 - i1 + G3;
    y1 = y0 - j1 + G3;
    z1 = z0 - k1 + G3;
    x2 = x0 - i2 + 2.0*G3;
    y2 = y0 - j2 + 2.0*G3;
    z2 = z0 - k2 + 2.0*G3;
    x3 = x0 - 1.0 + 3.0*G3;
    y3 = y0 - 1.0 + 3.0*G3;
    z3 = z0 - 1.0 + 3.0*G3;

    ii = i & 255;
    jj = j & 255;
    kk = k & 255;
    
    gi0 = perm[ii+perm[jj+perm[kk]]] % 12;
    gi1 = perm[ii+i1+perm[jj+j1+perm[kk+k1]]] % 12;
    gi2 = perm[ii+i2+perm[jj+j2+perm[kk+k2]]] % 12;
    gi3 = perm[ii+1+perm[jj+1+perm[kk+1]]] % 12;
    
    t0 = 0.6 - x0*x0 - y0*y0 - z0*z0;
    if(t0<0){
         n0 = 0.0;
    }
    else{
        t0 *= t0;
        n0 = t0 * t0 * dot(x0, y0, z0, grad[gi0]);
    }

    t1 = 0.6 - x1*x1 - y1*y1 - z1*z1;
    if(t1<0){
         n1 = 0.0;
    }
    else{
        t1 *= t1;
        n1 = t1 * t1 * dot(x1, y1, z1, grad[gi1]);
    }

    t2 = 0.6 - x2*x2 - y2*y2 - z2*z2;
    if(t2<0){
         n2 = 0.0;
    }
    else{
        t2 *= t2;
        n2 = t2 * t2 * dot(x2, y2, z2, grad[gi2]);
    }

    t3 = 0.6 - x3*x3 - y3*y3 - z3*z3;
    if(t3<0){
         n3 = 0.0;
    }
    else{
        t3 *= t3;
        n3 = t3 * t3 * dot(x3, y3, z3, grad[gi3]);
    }

    return 16.0*(n0 + n1 + n2 + n3)+1.0;
}

inline float simplex_noise(int octaves, float x, float y, float z){
    float value = 0.0;
    int i;
    for(i=0; i<octaves; i++){
        value += noise(
            x*pow(2, i),
            y*pow(2, i),
            z*pow(2, i)
        );
    }
    return value;
}

inline static void floating_rock(unsigned int x,unsigned int y,unsigned int z, UINT8* data, unsigned int side)
{
    float caves, center_falloff, plateau_falloff, density;
	float local_x = x*1.0f/(side-1);
	float local_y = y*1.0f/(side-1);
	float local_z = z*1.0f/(side-1);

     if(local_y <= 0.8){
            plateau_falloff = 1.0;
        }
        else if(0.8 < local_y && local_y < 0.9){
            plateau_falloff = 1.0-(local_y-0.8)*10.0;
        }
        else{
            plateau_falloff = 0.0;
        }

        center_falloff = 0.2/(
            pow((local_x-0.5)*1.5, 2) +
            pow((local_y-1.0)*0.8, 2) +
            pow((local_z-0.5)*1.5, 2)
        );
        caves = pow(simplex_noise(1, local_x*5, local_y*5, local_z*5), 3);
        density = (
            simplex_noise(5, local_x, local_y*0.5, local_z) *
            center_falloff *
            plateau_falloff
        );
        density *= pow(
            noise((local_x+1)*3.0, (local_y+1)*3.0, (local_z+1)*3.0)+0.4, 1.8
        );
        if(caves<0.5){
            density = 0;
        }
		data[x*side*side + y*side + z] = density>3.1 ? 255 : 0;
}

///


GridModel::GridModel( int power )
{
	dimm = 1<<power; //grid dimension
	size = dimm*dimm*dimm;//total size
	half_dimm = dimm>>1;
	_cells = new UINT8[size];//cells - voxels.
	_interacted = new bool[size];//array to store bool - if voxel was changed during this frame.
	memset( _interacted, 0, size*sizeof(bool) );

	power_for_chunk = max( unsigned(power-4), unsigned(4) );//chunk_size
	unsigned int _chunk_power = power - power_for_chunk;
	chunk_dimm = 1<<_chunk_power;//dimension for array of chunk
	chunk_size = chunk_dimm*chunk_dimm*chunk_dimm;
	_chunks = new VoxelChunk*[chunk_size];

	internal_chunk_size = 1<<power_for_chunk;//internal chunk size, dimm == chunk_dimm*internal_chunk_size

	Point center;//center of voxel is always (+-xx.5, +-yy.5, +-zz.5)

	int iter = 0;
	unsigned int tmp1, tmp2, tmp3;
	float radius = 0.0f;
	for (  int i = 0; i < dimm; i++ )
	{
		center.coord[0] = float(i - half_dimm);//well, but not here =)
		for (  int j = 0; j < dimm; j++ )
		{
			center.coord[1] = float(j - half_dimm);
			for (  int k = 0; k < dimm; k++ )
			{
				center.coord[2] = float(k - half_dimm);
				iter = i*dimm*dimm+ j*dimm+ k;

				_cells[iter] = 0;
				radius = sqrtf(  center.coord[0]*center.coord[0] + center.coord[1]*center.coord[1] + center.coord[2]*center.coord[2]);
				if ( radius < dimm/2 - 1 )
					_cells[iter] = 255;
				//floating_rock(i, j, k, _cells, dimm);
				if ( iter != GetCellIndex(center, tmp1, tmp2, tmp3) )
				{
					std::cout<<"Error!";
				}
			}
		}
	}

	int tmp_lbl[3];
	int tmp_ufr[3];
	float _h_s = float(internal_chunk_size>>1);
	for (  int i = 0; i < chunk_dimm; i++ )
	{
		tmp_lbl[0] = int(i<<power_for_chunk) - int(dimm/2);
		center.coord[0] = float(tmp_lbl[0]) + _h_s;//but here it is
		tmp_ufr[0] = tmp_lbl[0] + internal_chunk_size;
		for (  int j = 0; j < chunk_dimm; j++ )
		{
			tmp_lbl[1] = int(j<<power_for_chunk) - int(dimm/2);
			center.coord[1] = float(tmp_lbl[1]) + _h_s;
			tmp_ufr[1] = tmp_lbl[1] + internal_chunk_size;
			for (  int k = 0; k < chunk_dimm; k++ )
			{
				tmp_lbl[2] = int(k<<power_for_chunk) - int(dimm/2);
				center.coord[2] = float(tmp_lbl[2]) + _h_s;
				tmp_ufr[2] = tmp_lbl[2] + internal_chunk_size;

				iter = i*chunk_dimm*chunk_dimm+ j*chunk_dimm+ k;
				
				_chunks[ iter ] = new VoxelChunk( center, tmp_lbl, tmp_ufr );
				_chunks[ iter ]->MarkDirty();
				_dirty_chunks.push_back( _chunks[ iter ] );
			}
		}
	}

	/* init mutex*/
#ifdef USE_SPINLOCK
	pthread_spin_init(&spinlock, 0);
#else
	pthread_mutex_init(&mutex, NULL);
#endif
	
}


void GridModel::ReInitModel( bool clear )
{
	unsigned int iter;
	for (  int i = 0; i < dimm; i++ )
	{
		for (  int j = 0; j < dimm; j++ )
		{
			for (  int k = 0; k < dimm; k++ )
			{
				iter = i*dimm*dimm+ j*dimm+ k;

				_cells[iter] = clear ? 0 : 255;
				_interacted[iter] = false;
				//floating_rock(i, j, k, _cells, dimm);				
			}
		}
	}

	_dirty_chunks.clear();
	_modified_chunks.clear();
	for ( unsigned int i = 0; i < chunk_size; i++ )
	{
		_dirty_chunks.push_back(_chunks[i]);
	}
}


unsigned int GridModel::GetDimm()
{
	return dimm;
}

inline  unsigned int GridModel::GetCellIndex( const Point& pos, unsigned int &x, unsigned int &y, unsigned int &z )
{
	x = unsigned( int(pos.coord[0]) + half_dimm );
	y = unsigned( int(pos.coord[1]) + half_dimm );
	z = unsigned( int(pos.coord[2]) + half_dimm );

	return x*dimm*dimm + y*dimm + z;
}


void GridModel::UpdateGrid()
{
	unsigned int i, j;
	unsigned int index = 0;
	unsigned int x,y,z;

	bool not_dirty;
	for( i = 0; i < _modified_chunks.size(); i++ )//bad
	{
		not_dirty = true;
		for( j = 0; j < _dirty_chunks.size(); j++ )
		{
			if ( _dirty_chunks[j] == _modified_chunks[i] )
			{
				not_dirty = false;
				break;
			}
		}

		if (not_dirty)
			_modified_chunks[i]->RecalcColor( _cells, dimm );
	
	}

	_modified_chunks.clear();

	for( i = 0; i < _dirty_chunks.size(); i++ )
	{
		index = GetCellIndex( (_dirty_chunks[i]->GetCenter() ), x, y, z);
				
		_dirty_chunks[i]->CreateMesh( _cells, _interacted, dimm );
		if ( _dirty_chunks[i]->GetVAO() != NULL )// If mesh creating was successfull.
		{
			_renderable_chunks[index] = _dirty_chunks[i]->GetVAO();			
			_modified_chunks.push_back(_dirty_chunks[i]);
		}
		else
			_renderable_chunks.erase( index );
	}

	_dirty_chunks.clear();
}


void GridModel::EnshureMarked(int i, int j, int k)
{
	VoxelChunk* ptr = _chunks[ (i>>power_for_chunk)*chunk_dimm*chunk_dimm + (j>>power_for_chunk)*chunk_dimm + (k>>power_for_chunk) ];

	if ( !(ptr->IsDirty()) )
	{
		ptr->MarkDirty();
		_dirty_chunks.push_back(ptr);
	}
	// + side chunks
	//why not in the prev. "if" - you can modify one more cell at border of dirty chunk, so you have to update 
	//i
	if ( i && (i%internal_chunk_size == 0) )
	{
		ptr = _chunks[ ((i-1)>>power_for_chunk)*chunk_dimm*chunk_dimm + (j>>power_for_chunk)*chunk_dimm + (k>>power_for_chunk) ];
		if ( !(ptr->IsDirty()) )
		{
			ptr->MarkDirty();
			_dirty_chunks.push_back(ptr);
		}
	}
	if ( (i<(dimm-internal_chunk_size)) && ((i+1)%internal_chunk_size == 0) )
	{
		ptr = _chunks[ ((i+1)>>power_for_chunk)*chunk_dimm*chunk_dimm + (j>>power_for_chunk)*chunk_dimm + (k>>power_for_chunk) ];
		if ( !(ptr->IsDirty()) )
		{
			ptr->MarkDirty();
			_dirty_chunks.push_back(ptr);
		}
	}
	//j
	if ( j && (j%internal_chunk_size == 0) )
	{
		ptr = _chunks[ ((i)>>power_for_chunk)*chunk_dimm*chunk_dimm + ((j-1)>>power_for_chunk)*chunk_dimm + (k>>power_for_chunk) ];
		if ( !(ptr->IsDirty()) )
		{
			ptr->MarkDirty();
			_dirty_chunks.push_back(ptr);
		}
	}
	if ( (j<(dimm-internal_chunk_size)) && ((j+1)%internal_chunk_size == 0) )
	{
		ptr = _chunks[ ((i)>>power_for_chunk)*chunk_dimm*chunk_dimm + ((j+1)>>power_for_chunk)*chunk_dimm + (k>>power_for_chunk) ];
		if ( !(ptr->IsDirty()) )
		{
			ptr->MarkDirty();
			_dirty_chunks.push_back(ptr);
		}
	}
	//k
	if ( k && (k%internal_chunk_size == 0) )
	{
		ptr = _chunks[ ((i)>>power_for_chunk)*chunk_dimm*chunk_dimm + ((j)>>power_for_chunk)*chunk_dimm + ((k-1)>>power_for_chunk) ];
		if ( !(ptr->IsDirty()) )
		{
			ptr->MarkDirty();
			_dirty_chunks.push_back(ptr);
		}
	}
	if ( (k<(dimm-internal_chunk_size)) && ((k+1)%internal_chunk_size == 0) )
	{
		ptr = _chunks[ ((i)>>power_for_chunk)*chunk_dimm*chunk_dimm + ((j)>>power_for_chunk)*chunk_dimm + ((k+1)>>power_for_chunk) ];
		if ( !(ptr->IsDirty()) )
		{
			ptr->MarkDirty();
			_dirty_chunks.push_back(ptr);
		}
	}

}


int GridModel::UpdateCellMelt( int i, int j, int k, UINT8 val )
{
	VoxelChunk* ptr = _chunks[ (i>>power_for_chunk)*chunk_dimm*chunk_dimm + (j>>power_for_chunk)*chunk_dimm + (k>>power_for_chunk) ];
	
	if (1)//if surface interaction.
	{
		/// If chunk is not visible - do nothing
		if ( ptr->GetVAO() == NULL )
			return 0;

	
		// zero alpha means that either voxel is not visible or is empty
		UINT8 alpha =  ptr->GetVoxelAlpha( i - (i>>power_for_chunk)*internal_chunk_size,
			j - (j>>power_for_chunk)*internal_chunk_size , k - (k>>power_for_chunk)*internal_chunk_size );//local index in chunk.

		if (  alpha == 0 )
			return 0;
	}
	
#ifdef USE_SPINLOCK
	pthread_spin_lock(&spinlock);
#else
	pthread_mutex_lock(&mutex);
#endif
	
	_cells[i*dimm*dimm+ j*dimm+ k] = ( _cells[i*dimm*dimm+ j*dimm+ k] > val ) ? (_cells[i*dimm*dimm+ j*dimm+ k] - val) : 0;
	_interacted[i*dimm*dimm+ j*dimm+ k] = true;

	EnshureMarked(i, j, k);
#ifdef USE_SPINLOCK
	pthread_spin_unlock(&spinlock);
#else
	pthread_mutex_unlock(&mutex);
#endif

	return 1;
}


int GridModel::UpdateCellAdd( int i, int j, int k, UINT8 val )
{
	VoxelChunk* ptr = _chunks[ (i>>power_for_chunk)*chunk_dimm*chunk_dimm + (j>>power_for_chunk)*chunk_dimm + (k>>power_for_chunk) ];
	
	UINT8* current_voxel_ptr = &_cells[i*dimm*dimm+ j*dimm+ k];

	if (1)//if surface interaction.
	{
		/*unsigned int index_x = i - (i>>power_for_chunk)*internal_chunk_size;
		unsigned int index_y = j - (j>>power_for_chunk)*internal_chunk_size;
		unsigned int index_z = k - (k>>power_for_chunk)*internal_chunk_size; //local index in chunk.*/

		UINT8 alpha = ptr->GetVoxelAlpha( i - (i>>power_for_chunk)*internal_chunk_size,
			j - (j>>power_for_chunk)*internal_chunk_size , k - (k>>power_for_chunk)*internal_chunk_size );//local index in chunk.

		if (  alpha == 0 && *current_voxel_ptr != 0 )// if not visible and not empty
			return 0;
	}

#ifdef USE_SPINLOCK
	pthread_spin_lock(&spinlock);
#else
	pthread_mutex_lock(&mutex);
#endif

	if ( *current_voxel_ptr != 255 )
		*current_voxel_ptr = ( *current_voxel_ptr + val ) < 255 ? *current_voxel_ptr + val : 255;		
	else
		return 0;

	_interacted[i*dimm*dimm+ j*dimm+ k] = true;
	EnshureMarked(i, j, k);

#ifdef USE_SPINLOCK
	pthread_spin_unlock(&spinlock);
#else
	pthread_mutex_unlock(&mutex);
#endif

	return 1;
}


GridModel::~GridModel()
{
	delete [] _cells;

	for ( int i = 0; i < chunk_dimm*chunk_dimm*chunk_dimm; i++ )
		delete _chunks[i];

	delete [] _chunks;
	delete [] _interacted;
#ifdef USE_SPINLOCK
    pthread_spin_destroy(&spinlock);
#else
    pthread_mutex_destroy(&mutex);
#endif

}

UINT8* GridModel::GetCells()
{
	return _cells;
}

unsigned int GridModel::GetSize()
{
	return size;
}

std::map< unsigned int, VAO* >* GridModel::GetRenderableCells()
{
	return &_renderable_chunks;
}