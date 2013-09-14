#pragma once
//Incapsulates all necessary objects for kinect interaction + visual representation
#include "main.h"
#include <pthread.h>
#include <sched.h> /* I think phreads-w32 has a built in way for getting the processor count */

class TriangleMesh;
class KinectReader;
class GridModel;
class VAO;
class Shader;

class KinectTool
{
public:
	KinectTool(float half_x, float half_y, float start_z, float end_z);
	~KinectTool();
	void DoToolUpdate();
	int InteractModel(GridModel* model, glm::quat quat);
	TriangleMesh* GetToolMesh();
	Shader* GetToolShader();
	unsigned int GetPVMLocation();
	
	pthread_barrier_t barrier;

private:
	KinectTool();
	float _start_z;
	float _end_z;
	
	TriangleMesh* _msh;
	KinectReader* _reader;
	float* _tmp_blured_image;
	Shader* _tool_shader;
	int pvmLocMesh;
	
	int cpu_count;
	pthread_t* threads;

	void parallellise(long cpu, long beginning, long stop);
};

