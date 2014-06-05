#include "KinectTool.h"

#define DIR_Z_STEP 1.0f;
#define FALLBACK_CPU_COUNT 4


static Point* lastPoints;

KinectTool::KinectTool( float half_x, float half_y, float start_z, float end_z )
{
	PAD_DEPTH = 0.05f; // CentiMeters

	/* Creats a vertex grid mesh out of triengels */
	_msh = new TriangleMesh(640, 480, -half_x, -half_y, half_x, half_y, start_z);
	printf("Kinect grid created. \n");

	/* Initilizing size values of grid.*/
	_start_z = start_z;
	_end_z = end_z;
	int start_d = 800;

	/* Initilizes the kinect reader. */
	_reader = new KinectReader(start_d, start_d+int(_start_z - _end_z)*2, _start_z - _end_z );
	printf("Kinect Reader initilized. With min_depth 800, max_depth, dist. \n");

	/* Bluring grid image */ 
	_tmp_blured_image = new float[640*480];
	memset( _tmp_blured_image, 0, 640*480*sizeof(float) );

	/* Initilizing Kinect tool shaders */
	_tool_shader = new Shader();
	printf("Kinect Tool Shader: Initilized... ");
	_tool_shader->loadFragmentShader("Shaders/mesh.frag");
	printf("Kinect Tool \"Fregament\" Shader: Initilized... ");
	_tool_shader->loadGeometryShader("Shaders/mesh.geom");
	printf("Kinect Tool \"Geometery\" Shader: Initilized... ");
	_tool_shader->loadVertexShader("Shaders/mesh.vert");
	printf("Kinect Tool \"Vertex\" Shader: Initilized... \n");
	_tool_shader->link();
	printf("Kinect Tool Shader: Linked... \n");

	/*	Getting shader returns an integer that represents 
		the location of a specific uniform variable within 
		a program object.  */ 
	pvmLocMesh = glGetUniformLocation(_tool_shader->id(), "pvm");

	
	/* Get logical processor count */
	/*cpu_set_t cpu_set;
	int cpu_count = 0;
	CPU_ZERO(&cpu_set);
	sched_getaffinity(0, sizeof(cpu_set), &cpu_set);
	for (cpu_count = 1; cpu_count <= 480; cpu_count++)
	  if (CPU_ISSET(cpu_count, &cpu_set) == 0)
	    break;*/
	cpu_count = 8;
	cpu_count = ((cpu_count <= 0) && (cpu_count <= 480)) ? cpu_count : FALLBACK_CPU_COUNT;
	
	/* Initialise threads  */
	threads = (pthread_t*)malloc(this->cpu_count * sizeof(pthread_t));
	pthread_barrier_init(&(this->barrier), NULL, this->cpu_count + 1);
	printf("Initialise threads... \n");

	/* Start threads */
	long beginning = 0, stop, i;
	for (i = 1; i <= cpu_count; i++)
	{
		stop = i * 480 / cpu_count;
		parallellise(i - 1, beginning, stop);
		beginning = stop;
		//sched_setscheduler(0, SCHED_FIFO);
	}
	printf("Start threads... \n");
}

/**
 * Return this tool shader
 */
Shader* KinectTool::GetToolShader()
{
	return _tool_shader;
}
/**
 * Return lokation of mersh I gess
 */
unsigned int KinectTool::GetPVMLocation()
{
	return pvmLocMesh;
}

inline void Blur( float* in_out, float* tmp )
{
	//static double coeffs[] = {0.0545, 0.224, 0.4026, 0.224, 0.0545};
	static float coeffs[] = {0.2f, 0.2f, 0.2f, 0.2f, 0.2f};
	float summ = 0;
	//X
	int i,j,dx;
	for ( j = 2; j < 478; j++ )	
	{
		for ( i = 0; i < 640; i++ )
		{
			tmp[ j*640 + i ] = 0.0f;
			for ( dx = -2; dx < 3; dx++ )
			{
				tmp[ j*640 + i ] += coeffs[dx+2]*in_out[ (j+dx)*640 + i ];
			}
		}
	}	
	//Y
	for ( j = 2; j < 478; j++ )
	{
		for ( i = 2; i < 638; i++ )
		{
			in_out[ j*640 + i ] = 0.0f;
			for ( dx = -2; dx < 3; dx++ )
			{
				in_out[ j*640 + i ] += (coeffs[dx+2]*tmp[ j*640 + i +dx]);
			}
		}
	}
}

const int eventCount = 1;
HANDLE hEvents[eventCount];

void KinectTool::DoToolUpdate()
{
	/* Creats an array of depth */
	_reader->ProcessDepth();//both can be paralelized, but
	//sync point here for process depth.
	//memfence, in a way
	
	/* Gets the depth map from the last frame */
	float* depth_ptr = _reader->GetDepth(); //Flikers
	
	//Blur.	
	//Blur( depth_ptr, _tmp_blured_image );//Makes it emen more flikering, but it should be done.
	//memfence again
	
	/* Uppdates the trinegel mesh */
	//_msh->UpdateDepth( depth_ptr );
	_msh->UpdateDepth( _reader->GetHeadTracking(), _reader->GetDepthVector() );
	
	//memfence again
	//or you can combine both calls into one ( basically same happens there ), BUT with blur I see no way to do it.
	//So the best way to parralelize it - SIMD-like paralelization inside of each function.

	// Wait for 0ms, just quickly test if it is time to process a skeleton
    if ( WAIT_OBJECT_0 == WaitForSingleObject(_reader->m_hNextSkeletonEvent, 0) )
        _reader->ProcessSkeleton();
}

inline Point Rotate( const Point& v, const glm::quat& q )//looks to be fast.
{
	//quat - point cross
	Point res;
	res.coord[0] = q.y * v.coord[2] - v.coord[1] * q.z;
	res.coord[1] = q.z * v.coord[0] - v.coord[2] * q.x;
	res.coord[2] = q.x * v.coord[1] - v.coord[0] * q.y;
	//again cross
	Point res1;
	res1.coord[0] = q.y * res.coord[2] - res.coord[1] * q.z;
	res1.coord[1] = q.z * res.coord[0] - res.coord[2] * q.x;
	res1.coord[2] = q.x * res.coord[1] - res.coord[0] * q.y;
	//
	res.coord[0] *= 2.0f*q.w;
	res.coord[1] *= 2.0f*q.w;
	res.coord[2] *= 2.0f*q.w;
	//
	res1.coord[0] *= 2.0f;
	res1.coord[1] *= 2.0f;
	res1.coord[2] *= 2.0f;
	
	res.coord[0] += res1.coord[0] + v.coord[0];
	res.coord[1] += res1.coord[1] + v.coord[1];
	res.coord[2] += res1.coord[2] + v.coord[2];
	return res;
}

inline Point Transform( const Point& v, const glm::mat4& m)
{
	glm::vec4 p (v.coord[0], v.coord[1], v.coord[2], 1.0f);
	p = m*p;
	Point res;
	res.coord[0] = p.x;
	res.coord[1] = p.y;
	res.coord[2] = p.z;

	return res;
}


static glm::quat inverse;
static glm::mat4 modelMatrix;
static Point* points;
static unsigned int grid_dimm;
static Point dir_vector;
static GridModel* grid_model;
static int cpu_output[480];
static float voxel_distance;

void KinectTool::StartInteractModel( GridModel* model, glm::quat quat, glm::mat4 modelM, float modelSide)
{
	//for loop for each point, rotated by inverse of quat
	points = _msh->GetPoints();
	inverse = glm::conjugate(quat);
	modelMatrix = glm::inverse(modelM);
	
	voxel_distance = modelSide*2.0f / float(model->GetDimm());

	//std::cout << "VoxelDistance = " << voxel_distance << ", PAD_DEPTH =" << PAD_DEPTH << ", Tool iterations = " << PAD_DEPTH/voxel_distance << std::endl;
	
	//Can be easy paralelized. With one but - UpdateCell should be treated properly - it's nor thead safe for moment.
	grid_dimm = model->GetDimm() - 1;

	dir_vector.coord[0] = 0.0f;
	dir_vector.coord[1] = 0.0f;
	dir_vector.coord[2] = DIR_Z_STEP;
	dir_vector = Rotate(dir_vector, inverse );
	grid_model = model;

	//PAD_DEPTH = ((float)model->GetDimm())/256.0f * 25.0f;
	
	pthread_barrier_wait(&(this->barrier)); /* Start threads */
}

int KinectTool::StopInteractModel()
{
	int accum = 0;

	for (long cpu = 0; cpu < this->cpu_count; cpu++)
	  accum += cpu_output[cpu];
	
	pthread_barrier_wait(&(this->barrier)); /* Wait for threads */
	
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
	delete [] lastPoints;
	
	pthread_barrier_destroy(&(this->barrier));
	free(threads);
}



void* SpeechThreed(void* args)
{
	void** args_ = (void**)args;
	KinectTool* self = (KinectTool*)(args_[0]);
	Input* input = (Input*)(args_[1]);
	free(args);

	printf("Thread started. \n");

	for(;;)
	{
		/* Speech Start */
		hEvents[0] = self->_reader->m_hSpeechEvent;

		// Check to see if we have either a message (by passing in QS_ALLINPUT)
		// Or a speech event (hEvents)
		DWORD dwEvent = MsgWaitForMultipleObjectsEx(eventCount, hEvents, INFINITE, QS_ALLINPUT, MWMO_INPUTAVAILABLE);

		// Check if this is an event we're waiting on and not a timeout or message
		if (WAIT_OBJECT_0 == dwEvent)
		{
			// DEMO
			// self->_reader->ProcessSpeech(input);
		}
	/* Speech end */
	}

	return NULL;
}

void KinectTool::parallellise(long cpu, long beginning, long stop)
{
	long args_0 = (beginning | (stop << 9) | (cpu << 18));
	long args_1 = (long)(this);
	long* args = new long[2];
	args[0] = args_0;
	args[1] = args_1;
	if ((errno = pthread_create(this->threads + cpu, NULL, run, args)))
	{
		perror("pthread_create");
		abort();
	}
}


inline int direction(int from, int dir) {
	int aux;

	switch (dir) {
	case 2:
		return from;
	case 0: // up
		aux = from - 1;
		return (aux < 0)? -1 : aux;
	case 3: // down
		aux = from + 1;
		return (aux/480 != from/480)? -1 : aux;
	case 1: // left
		aux = from - 480;
		return (aux < 0)? -1 : aux;
	case 4: // right
		aux = from + 480;
		return (aux >= 480*640)? -1 : aux;
	}

	return dir;
}

static void* run(void* args)
{
	KinectTool* tool = (KinectTool*)(((long*)args)[1]);

	long beginning = long(((long*)args)[0]) & 511;
	long stop = (long(((long*)args)[0]) >> 9) & 511;
	long cpu = (long(((long*)args)[0]) >> 18) & 511;
	delete args;

	long count, x, y;
	int accum = 0;
  
	Point action_point, action_base;
	unsigned int tmp1, tmp2, tmp3;
	Point tmp, last_action_dir;
	UINT8 val = 128;
	
	glm::vec3 action_p, towards_p, left_p, up_p, base_p, current_p;
	glm::vec3 current_dir, left_dir, up_dir, tool_dir;
	int tool_iterations;
	int max_steps = 100;
  
	for (;;)
	{
		pthread_barrier_wait(&(tool->barrier)); /* Wait for start cue */
		accum = 0;
		Point local_dir_vector = dir_vector;
		tool_dir = glm::vec3(dir_vector.coord[0], dir_vector.coord[1], dir_vector.coord[2]);
		tool_dir = glm::normalize(tool_dir);
		
		count = 0;
		
		tool_iterations = int(tool->PAD_DEPTH / voxel_distance);
		if (tool_iterations == 0) tool_iterations = 1;
		max_steps = tool_iterations * 2.0f;
		
		for (y = beginning; y < stop; y++)
		for (x = 0; x < 640; x++)
		{
			//tmp = points[ x*480 + y ];
			//tmp.coord[2] -= DIR_Z_STEP * PAD_DEPTH;
			//action_point = Rotate( points[ x*480 + y ], inverse);
			action_point = Transform (points[ x*480 + y ], modelMatrix);
			action_p = glm::vec3(action_point.coord[0], action_point.coord[1], action_point.coord[2]);
			/*for ( int delta = 0; delta < tool->PAD_DEPTH; delta++ )
			{
				tmp.coord[0] = action_point.coord[0] + local_dir_vector.coord[0]*delta;
				tmp.coord[1] = action_point.coord[1] + local_dir_vector.coord[1]*delta;
				tmp.coord[2] = action_point.coord[2] + local_dir_vector.coord[2]*delta;
				grid_model->GetCellIndex(tmp, tmp1, tmp2, tmp3);
		
				if (!( ( tmp1 > grid_dimm ) || ( tmp2 > grid_dimm ) || ( tmp3 > grid_dimm )))//if we are in model bounds
					accum += grid_model->UpdateCellMelt(tmp1, tmp2, tmp3, val);
				else
					break;
			}*/
			
			
			// checking up and left
			int up = direction(x*480 + y, 0);
			int left = direction(x*480 + y, 1);
			int up_steps = 1;
			int left_steps = 1;

			if (up >= 0) {
				action_base = action_base = Transform (points[up], modelMatrix);
				up_p = glm::vec3(action_base.coord[0], action_base.coord[1], action_base.coord[2]);

				up_dir = up_p - action_p;
				up_steps = int(glm::length(up_dir));
				// Max steps?
				if (up_steps < 1) up_steps = 1;
				else {
					up_dir = glm::normalize(up_dir);

					if (up_steps > max_steps) up_steps = 1;
					//up_steps = 2;
				}
			}
			else up_dir = glm::vec3(0.0f);

			if (left >= 0) {
				action_base = action_base = Transform (points[left], modelMatrix);
				left_p = glm::vec3(action_base.coord[0], action_base.coord[1], action_base.coord[2]);

				left_dir = left_p - action_p;
				left_steps = int(glm::length(left_dir));
				// Max steps?
				if (left_steps < 1) left_steps = 1;
				else {
					left_dir = glm::normalize(left_dir);

					if (left_steps > max_steps) left_steps = 1;
					//left_steps = 2;
				}
			}
			else left_dir = glm::vec3(0.0f);

			for (int i = 0; i < up_steps; ++i) {
				for (int j = 0; j < left_steps; ++j) {
					base_p = action_p + up_dir*float(i) + left_dir*float(j);

					for (int k = 0; k < tool_iterations; ++k) {
						current_p = base_p + tool_dir*float(k);
						
						memcpy(&tmp.coord, &current_p[0], 3*sizeof(float));
						
						grid_model->GetCellIndex(tmp, tmp1, tmp2, tmp3);
				
						if (!( ( tmp1 > grid_dimm ) || ( tmp2 > grid_dimm ) || ( tmp3 > grid_dimm )))//if we are in model bounds
							accum += grid_model->UpdateCellMelt(tmp1, tmp2, tmp3, val);
						else
							break;
					}
				}
			}
			

			/*
			// Base point check
			base_p = action_p;
			for (int k = 0; k < tool_iterations; ++k) {
				current_p = base_p + tool_dir*float(k);

				memcpy(&tmp.coord, &current_p[0], 3*sizeof(float));

				grid_model->GetCellIndex(tmp, tmp1, tmp2, tmp3);

				if (!( ( tmp1 > grid_dimm ) || ( tmp2 > grid_dimm ) || ( tmp3 > grid_dimm )))//if we are in model bounds
					accum += grid_model->UpdateCellMelt(tmp1, tmp2, tmp3, val);
				else
					break;
			}

			// checking up and left
			for (int i = 0; i <= 1; ++i) { 
				int current = direction(x*480 + y, i);

				if (current < 0) continue;

				action_base = Transform (points[current], modelMatrix);
				towards_p = glm::vec3(action_base.coord[0], action_base.coord[1], action_base.coord[2]);

				current_dir = towards_p - action_p;

				int steps = int(glm::length(current_dir));
				if (steps > max_steps) steps = max_steps;

				current_dir = glm::normalize(current_dir);

				for (int j = 1; j < steps; ++j) {
					base_p = action_p + current_dir*float(j);

					for (int k = 0; k < tool_iterations; ++k) {
						current_p = base_p + tool_dir*float(k);

						memcpy(&tmp.coord, &current_p[0], 3*sizeof(float));

						grid_model->GetCellIndex(tmp, tmp1, tmp2, tmp3);

						if (!( ( tmp1 > grid_dimm ) || ( tmp2 > grid_dimm ) || ( tmp3 > grid_dimm )))//if we are in model bounds
							accum += grid_model->UpdateCellMelt(tmp1, tmp2, tmp3, val);
						else
							break;
					}
				}
			}
			*/
		}
      
		  cpu_output[cpu] = accum;
      
		  pthread_barrier_wait(&(tool->barrier)); /* Signal that thread complete */
    }
  return NULL;
}

