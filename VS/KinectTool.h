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

void* SpeechThreed(void*);

static void* run(void* beginning_stop_cpu);

class KinectTool
{
public:
	KinectTool( float half_x, float half_y, float start_z, float end_z );
	~KinectTool();
	void DoToolUpdate();
	void StartInteractModel( GridModel* model, glm::quat quat );
	int StopInteractModel( );
	TriangleMesh* GetToolMesh();
	Shader* GetToolShader();
	unsigned int GetPVMLocation();

	pthread_barrier_t barrier;
	KinectReader* _reader;

private:
	KinectTool();

	float _start_z, _end_z;

	TriangleMesh* _msh;
	float* _tmp_blured_image;
	Shader* _tool_shader;
	int pvmLocMesh;	

	int cpu_count;
	pthread_t* threads;
	
	void parallellise(long cpu, long beginning, long stop);

};

