#include "KinectTool.h"
#include "TriangleMesh.h"
#include "KinectReader.h"
#include "GridModel.h"
#include "Shader.h"
#include "GraphicsLib.h"

KinectTool::KinectTool()
{
}

KinectTool::KinectTool(float half_x, float half_y, float start_z, float end_z)
{
	_msh = new TriangleMesh(640, 480, -half_x, -half_y, half_x, half_y, start_z);
	_start_z = start_z;
	_end_z = end_z;
	int start_d = 800;
	
	_reader = new KinectReader(start_d, start_d + (int)(_start_z - _end_z) * 2, _start_z - _end_z );
	
	_tmp_blured_image = new float[640 * 480];
	memset(_tmp_blured_image, 0, 640 * 480 * sizeof(float));
	
	_tool_shader = new Shader();
	_tool_shader->loadFragmentShader("Shaders/mesh.frag");
	_tool_shader->loadGeometryShader("Shaders/mesh.geom");
	_tool_shader->loadVertexShader("Shaders/mesh.vert");
	_tool_shader->link();
	pvmLocMesh = glGetUniformLocation(_tool_shader->id(), "pvm");
}

Shader* KinectTool::GetToolShader()
{
	return _tool_shader;
}

unsigned int KinectTool::GetPVMLocation()
{
	return pvmLocMesh;
}

inline void Blur(float* in_out, float* tmp)
{
	//static double coeffs[] = {0.0545, 0.224, 0.4026, 0.224, 0.0545};
	static float coeffs[] = {0.2f, 0.2f, 0.2f, 0.2f, 0.2f};
	float summ = 0;
	//X
	int i, j, dx;
	for (j = 2; j < 478; j++)
		for (i = 0; i < 640; i++)
		{
			tmp[j * 640 + i] = 0.0f;
			for (dx = -2; dx < 3; dx++)
				tmp[j * 640 + i] += coeffs[dx + 2] * in_out[(j + dx) * 640 + i];
		}
	//Y
	for (j = 2; j < 478; j++) /* TODO: is this lake of symmetri correct */
		for (i = 2; i < 638; i++)
		{
			in_out[j * 640 + i] = 0.0f;
			for (dx = -2; dx < 3; dx++)
				in_out[j * 640 + i] += (coeffs[dx + 2] * tmp[j * 640 + i + dx]);
		}
}

void KinectTool::DoToolUpdate()
{
	_reader->ProcessDepth(); //both can be paralelized, but
	//sync point here for process depth.
	//memfence, in a way
	float* depth_ptr = _reader->GetDepth(); //Flikers
	//Blur.	
	//Blur(depth_ptr, _tmp_blured_image); //Makes it emen more flikering, but it should be done.
	//memfence again
	_msh->UpdateDepth(depth_ptr);
	//memfence again
	//or you can combine both calls into one (basically same happens there), BUT with blur I see no way to do it.
	//So the best way to parralelize it - SIMD-like paralelization inside of each function.
}

inline Point Rotate(const Point& v, const glm::quat& q) //looks to be fast.
{
	//quat - point cross
	Point res;
	res.coord[0] = q.y * v.coord[2] - v.coord[1] * q.z;
	res.coord[1] = q.z * v.coord[0] - v.coord[2] * q.x;
	res.coord[2] = q.x * v.coord[1] - v.coord[0] * q.y;
	//again cross
	static Point res1;
	res1.coord[0] = q.y * res.coord[2] - res.coord[1] * q.z;
	res1.coord[1] = q.z * res.coord[0] - res.coord[2] * q.x;
	res1.coord[2] = q.x * res.coord[1] - res.coord[0] * q.y;
	//
	res.coord[0] *= 2.0f * q.w;
	res.coord[1] *= 2.0f * q.w;
	res.coord[2] *= 2.0f * q.w;
	//
	res1.coord[0] *= 2.0f;
	res1.coord[1] *= 2.0f;
	res1.coord[2] *= 2.0f;
	
	res.coord[0] += res1.coord[0] + v.coord[0];
	res.coord[1] += res1.coord[1] + v.coord[1];
	res.coord[2] += res1.coord[2] + v.coord[2];
	return res;
}


int KinectTool::InteractModel(GridModel* model, glm::quat quat)
{
	//for loop for each point, rotated by inverse of quat
	Point* points = _msh->GetPoints();
	glm::quat inverse = glm::conjugate(quat);
	
	
	//Can be easy paralelized. With one but - UpdateCell should be treated properly - it's nor thead safe for moment.
	Point action_point;
	unsigned int index = 0;
	unsigned int tmp1, tmp2, tmp3;
	unsigned int grid_size = model->GetSize();
	unsigned int grid_dimm = model->GetDimm() - 1;
	
	unsigned int pad_depth = 50; //grid_dimm;
	
        unsigned char val = 128;
	int accum = 0;
	Point tmp;
	Point dir_vector;
	const float dir_z_step = 1.0f;
	dir_vector.coord[0] = 0.0f;
	dir_vector.coord[1] = 0.0f;
	dir_vector.coord[2] = dir_z_step;
	dir_vector = Rotate(dir_vector, inverse);
	unsigned int tmp_pad = 0;
	for (unsigned int i = 0; i < 640; i++)
		for (unsigned int j = 0; j < 480; j++)
		{
			//tmp = points[i * 480 + j];
			//tmp.coord[2] -= dir_z_step*pad_depth;
			action_point = Rotate(points[i * 480 + j], inverse);
			for (unsigned int delta = 0; delta < pad_depth; delta++)
			{
				tmp.coord[0] = action_point.coord[0] + dir_vector.coord[0]*delta;
				tmp.coord[1] = action_point.coord[1] + dir_vector.coord[1]*delta;
				tmp.coord[2] = action_point.coord[2] + dir_vector.coord[2]*delta;
				index = model->GetCellIndex(tmp, tmp1, tmp2, tmp3);
				
				if ((tmp1 <= grid_dimm) && (tmp2 <= grid_dimm) && (tmp3 <= grid_dimm)) //if we are in model bounds
					accum += model->UpdateCellMelt(tmp1, tmp2, tmp3, val);
				else
					break;
			}
		}
	return accum;
}


TriangleMesh* KinectTool::GetToolMesh()
{
	return _msh;
}


KinectTool::~KinectTool()
{
	delete _msh;
	delete _reader;
	delete [] _tmp_blured_image;
	delete _tool_shader;
}

