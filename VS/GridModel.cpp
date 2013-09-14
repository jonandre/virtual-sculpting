#include <string.h>
#include "GridModel.h"
#include "VoxelChunk.h"
#include "VoxelBlock.h"
#include "main.h"


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

inline float dot(float x, float y, float z, float* g)
{
    return x * g[0] + y * g[1] + z * g[2];
}

inline float noise(float xin, float yin, float zin)
{
	float F3, G3, t, X0, Y0, Z0, x0, y0, z0, s, x1, y1, z1, x2, y2, z2, x3, y3, z3, t0, t1, t2, t3, n0, n1, n2, n3;
	int i, j, k, ii, jj, kk, i1, j1, k1, i2, j2, k2, gi0, gi1, gi2, gi3;
	
	F3 = 1.0f / 3.0f;
	s = (xin + yin + zin) * F3;
	i = (int)(xin + s);
	j = (int)(yin + s);
	k = (int)(zin + s);
	G3 = 1.0f / 6.0f;
	t = (i + j + k) * G3;
	X0 = i - t;
	Y0 = j - t;
	Z0 = k - t;
	x0 = xin - X0;
	y0 = yin - Y0;
	z0 = zin - Z0;
	
	#define __(I1, J1, K1, I2, J2, K2)  { i1 = I1; j1 = J1; k1 = K1; i2 = I2; j2 = J2; k2 = K2; }
	if (x0 >= y0)
	{
		if      (y0 >= z0)  __(1, 0, 0,  1, 1, 0)
		else if (x0 >= z0)  __(1, 0, 0,  1, 0, 1)
		else                __(0, 0, 1,  1, 0, 1)
	}
	else
	{
		if      (y0 < z0)  __(0, 0, 1,  0, 1, 1)
		else if (x0 < z0)  __(0, 1, 0,  0, 1, 1)
		else               __(0, 1, 0,  1, 1, 0)
	}
	#undef __
	
	x1 = x0 - i1 + G3;
	y1 = y0 - j1 + G3;
	z1 = z0 - k1 + G3;
	x2 = x0 - i2 + 2.0f * G3;
	y2 = y0 - j2 + 2.0f * G3;
	z2 = z0 - k2 + 2.0f * G3;
	x3 = x0 - 1.0f + 3.0f * G3;
	y3 = y0 - 1.0f + 3.0f * G3;
	z3 = z0 - 1.0f + 3.0f * G3;
	
	ii = i & 255;
	jj = j & 255;
	kk = k & 255;
	
	gi0 = perm[ii +      perm[jj +      perm[kk     ]]] % 12;
	gi1 = perm[ii + i1 + perm[jj + j1 + perm[kk + k1]]] % 12;
	gi2 = perm[ii + i2 + perm[jj + j2 + perm[kk + k2]]] % 12;
	gi3 = perm[ii + 1  + perm[jj + 1  + perm[kk + 1 ]]] % 12;
	
	#define __(I)                                                           \
		t##I = 0.6f - pow2(x##I) - pow2(y##I) - pow2(z##I);             \
		if (t##I < 0)                                                   \
			n##I = 0.0;                                             \
		else                                                            \
		{                                                               \
			t##I *= t##I;                                           \
			n##I = pow2(t##I) * dot(x##I, y##I, z##I, grad[gi##I]); \
		}
	
	__(0) __(1) __(2) __(3)
	#undef __
	
	return 16.0f * (n0 + n1 + n2 + n3) + 1.0f;
}

inline float simplex_noise(int octaves, float x, float y, float z)
{
	float value = 0.0;
	for (int i = 1, n = 1 << octaves; i != n; i <<= 1)
		value += noise(x * i, y * i, z * i);
	return value;
}

inline static void floating_rock(unsigned int x, unsigned int y, unsigned int z, unsigned char* data, unsigned int side)
{
	float caves, centre_falloff, plateau_falloff, density;
	float x_ = x / (side - 1.0f);
	float y_ = y / (side - 1.0f);
	float z_ = z / (side - 1.0f);
	
	if      (y_ <= 0.8)                 plateau_falloff = 1.0;
	else if ((0.8 < y_) && (y_ < 0.9))  plateau_falloff = 1.0f - (y_ - 0.8f) * 10.0f;
	else                                plateau_falloff = 0.0;
	
	#define __(X, Y)  pow((X) * Y, 2)
	centre_falloff = 0.2f / (__(x_ - 0.5f, 1.5f) + __(y_ - 1.0f, 0.8f) + __(z_ - 0.5f, 1.5f));
	#undef __
	
	caves = pow(simplex_noise(1, x_ * 5, y_ * 5, z_ * 5), 3);
	
	#define __(X)  (X + 1) * 3.0f
	density = simplex_noise(5, x_, y_ * 0.5f, z_) * centre_falloff * plateau_falloff;
	density *= pow(noise(__(x_), __(y_), __(z_)) + 0.4f, 1.8f);
	#undef __
	
	data[poly3(x, y, z, side)] = ((caves < 0.5) || (density <= 3.1)) ? 0 : 255;
}


GridModel::GridModel(int power)
{
	dimm = 1 << power; //grid dimension
	size = pow3(dimm); //total size
	half_dimm = dimm >> 1;
	_cells = new unsigned char[size]; //cells - voxels.
	_interacted = new bool[size]; //array to store bool - if voxel was changed during this frame.
	memset(_interacted, 0, size * sizeof(bool));
	
	power_for_chunk = max((unsigned int)(power - 4), (unsigned int)4); //chunk_size
	unsigned int _chunk_power = power - power_for_chunk;
	chunk_dimm = 1 << _chunk_power; //dimension for array of chunk
	chunk_size = pow3(chunk_dimm);
	_chunks = new VoxelChunk*[chunk_size];
	
	internal_chunk_size = 1 << power_for_chunk; //internal chunk size, dimm == chunk_dimm * internal_chunk_size
	
	Point center; //center of voxel is always (±xx.5, ±yy.5, ±zz.5)
	
	unsigned int iter = 0;
	unsigned int tmp1, tmp2, tmp3;
	float radius = 0.0f;
	for (int i = 0; i < (unsigned int)dimm; i++)
	{
		center.coord[0] = (float)(i - half_dimm);
		for (int j = 0; j < (unsigned int)dimm; j++)
		{
			center.coord[1] = (float)(j - half_dimm);
			for (int k = 0; k < (unsigned int)dimm; k++)
			{
				center.coord[2] = (float)(k - half_dimm);
				iter = poly3(i, j, k, dimm);
				
				_cells[iter] = 0;
				radius = sqrtf(pow2(center.coord[0]) + pow2(center.coord[1]) + pow2(center.coord[2]));
				if (radius < dimm / 2)
					_cells[iter] = 255;
				//floating_rock(i, j, k, _cells, dimm);
				if (iter != GetCellIndex(center, tmp1, tmp2, tmp3))
					std::cerr << "Error!" << std::endl;
			}
		}
	}
	
	int tmp_lbl[3];
	int tmp_ufr[3];
	float _h_s = (float)(internal_chunk_size >> 1);
	
	#define __(I, X)                                                                \
		tmp_lbl[I] = (int)(X << power_for_chunk) - (int)(dimm / 2);             \
		center.coord[I] = (float)(tmp_lbl[I]) + _h_s; /* but here it is */      \
		tmp_ufr[I] = tmp_lbl[I] + internal_chunk_size
	
	for (int i = 0; i < (unsigned int)chunk_dimm; i++)
	{
		__(0, i);
		for (int j = 0; j < (unsigned int)chunk_dimm; j++)
		{
			__(1, j);
			for (int k = 0; k < (unsigned int)chunk_dimm; k++)
			{
				__(2, k);
				iter = poly3(i, j, k, chunk_dimm);
				
				_chunks[iter] = new VoxelChunk(center, tmp_lbl, tmp_ufr);
				_chunks[iter]->MarkDirty();
				_dirty_chunks.push_back(_chunks[iter]);
			}
		}
	}
	#undef __
}


void GridModel::ReInitModel(bool clear)
{
	unsigned int iter;
	for (int i = 0; i < (unsigned int)dimm; i++)
		for (int j = 0; j < (unsigned int)dimm; j++)
			for (int k = 0; k < (unsigned int)dimm; k++)
			{
				iter = poly3(i, j, k, dimm);
				
				_cells[iter] = clear ? 0 : 255;
				_interacted[iter] = false;
				//floating_rock(i, j, k, _cells, dimm);
			}
	
	_dirty_chunks.clear();
	_modified_chunks.clear();
	for (unsigned int i = 0; i < chunk_size; i++)
		_dirty_chunks.push_back(_chunks[i]);
}


unsigned int GridModel::GetDimm()
{
	return dimm;
}

inline unsigned int GridModel::GetCellIndex(const Point& pos, unsigned int& x, unsigned int& y, unsigned int& z)
{
	x = (unsigned int)((int)(pos.coord[0]) + half_dimm);
	y = (unsigned int)((int)(pos.coord[1]) + half_dimm);
	z = (unsigned int)((int)(pos.coord[2]) + half_dimm);
	
	return poly3(x, y, z, dimm);
}


void GridModel::UpdateGrid()
{
	unsigned int i, j;
	unsigned int index = 0;
	unsigned int x, y, z;
	
	bool not_dirty;
	for (i = 0; i < _modified_chunks.size(); i++) //bad
	{
		not_dirty = true;
		for (j = 0; j < _dirty_chunks.size(); j++)
			if ( _dirty_chunks[j] == _modified_chunks[i])
			{
				not_dirty = false;
				break;
			}
		
		if (not_dirty)
			_modified_chunks[i]->RecalcColor(_cells, dimm);
	
	}
	
	_modified_chunks.clear();
	
	for (i = 0; i < _dirty_chunks.size(); i++)
	{
		index = GetCellIndex((_dirty_chunks[i]->GetCenter()), x, y, z);
		
		_dirty_chunks[i]->CreateMesh(_cells, _interacted, dimm);
		if (_dirty_chunks[i]->GetVAO() != NULL)// If mesh creating was successfull.
		{
			_renderable_chunks[index] = _dirty_chunks[i]->GetVAO();
			_modified_chunks.push_back(_dirty_chunks[i]);
		}
		else
			_renderable_chunks.erase(index);
	}
	
	_dirty_chunks.clear();
}

void GridModel::EnsureMarked(int i, int j, int k)
{
	VoxelChunk* ptr = _chunks[poly3_shift(i, j, k, chunk_dimm, power_for_chunk)];
	int limit = dimm - internal_chunk_size;
	
	#define on_clean()						\
		if (ptr->IsDirty() == false)		\
		{									\
			ptr->MarkDirty();				\
			_dirty_chunks.push_back(ptr);	\
		}
	
	on_clean()
	
	// + side chunks
	//why not in the prev. "if" - you can modify one more cell at border of dirty chunk, so you have to update 
	
	#define if_clean(cond_a, cond_modand, I, J, K)								\
		if ((cond_a) && ((cond_modand) % internal_chunk_size == 0))				\
		{																		\
			ptr = _chunks[poly3_shift(I, J, K, chunk_dimm, power_for_chunk)];	\
			on_clean()															\
		}
	
	if_clean(i,         i,     i - 1, j, k)
	if_clean(i < limit, i + 1, i + 1, j, k)
	
	if_clean(j,         j,     i, j - 1, k)
	if_clean(j < limit, j + 1, i, j + 1, k)
	
	if_clean(k,         k,     i, j, k - 1)
	if_clean(k < limit, k + 1, i, j, k + 1)
}


int GridModel::UpdateCellMelt(int i, int j, int k, unsigned char val)
{
	VoxelChunk* ptr = _chunks[poly3_shift(i, j, k, chunk_dimm, power_for_chunk)];
	
	if (1) //if surface interaction.
	{
		/// If chunk is not visible - do nothing
		if (ptr->GetVAO() == NULL)
			return 0;
		
		// zero alpha means that either voxel is not visible or is empty
		#define __(X)  (X - (X >> power_for_chunk) * internal_chunk_size)
		unsigned char alpha = ptr->GetVoxelAlpha(__(i), __(j), __(k)); //local index in chunk.
		#undef __
		
		if (alpha == 0)
			return 0;
	}
	
	int poly = poly3(i, j, k, dimm);
	_cells[poly] = (_cells[poly] > val) ? (_cells[poly] - val) : 0;
	_interacted[poly] = true;
	
	EnsureMarked(i, j, k);
	return 1;
}


int GridModel::UpdateCellAdd(int i, int j, int k, unsigned char val)
{
	VoxelChunk* ptr = _chunks[poly3_shift(i, j, k, chunk_dimm, power_for_chunk)];
	
	unsigned char* current_voxel_ptr = &_cells[poly3(i, j, k, dimm)];
	
	if (1) //if surface interaction.
	{
		#define __(X)  (X - (X >> power_for_chunk) * internal_chunk_size)
		unsigned char alpha = ptr->GetVoxelAlpha(__(i), __(j), __(k)); //local index in chunk.
		#undef __
		
		if ((alpha == 0) && *current_voxel_ptr) // if not visible and not empty
			return 0;
	}
	
	if (*current_voxel_ptr == 255)
		return 0;
	*current_voxel_ptr = (*current_voxel_ptr + val < 255) ? (*current_voxel_ptr + val) : 255;
	
	_interacted[poly3(i, j, k, dimm)] = true;
	EnsureMarked(i, j, k);
	return 1;
}


GridModel::~GridModel()
{
	delete [] _cells;
	
	for (int i = 0, n = pow3(chunk_dimm); i != n; i++)
		delete _chunks[i];
	
	delete [] _chunks;
	delete [] _interacted;
}

unsigned char* GridModel::GetCells()
{
	return _cells;
}

unsigned int GridModel::GetSize()
{
	return size;
}

std::map<unsigned int, VAO*>* GridModel::GetRenderableCells()
{
	return &_renderable_chunks;
}

