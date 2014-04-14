#include "StereoRender.h"

const float StereoRender::FOV = 30.0f;
const float StereoRender::ZNEAR = 0.1f;
const float StereoRender::ZFAR = 4048.f;

StereoRender::StereoRender() : shader(NULL)
{
	windowWidth = 512;
	windowHeight = 512;
	Point center;
	center.coord[0] = 0.0;
	center.coord[1] = 0.0;
	center.coord[2] = 0.0;
	Color clr;
	clr.comp[0] = 255;
	clr.comp[1] = 255;
	clr.comp[2] = 255;
	clr.comp[3] = 255;
	model_acted = spin_acted = camera_acted = rotate_acted = sound_acted =
		haptics_acted = camera_rotate_acted = false;

	for (int i = 0; i < 26; i++)
		actedIndex[i] = 1.0f;
}

StereoRender::~StereoRender()
{
	if (shader)
		delete shader;
}

void StereoRender::Init()
{
	SetupScene();
}

void StereoRender::SetupScene()
{
	printf("Setting up render scene... \n");

	/* Sets Screen Black and with and full alpha. */
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	printf("Gl blend function initiated with gl source alfa and gl one minus src alpha. \n");
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	// Enables blend
	glEnable(GL_DEPTH_TEST);

	// Loads Voxel Mesh shaders
	shader = new Shader();
	printf("Voxel Mesh Shader: Initilized... ");
	shader->loadFragmentShader("Shaders/shader.frag");
	printf("Voxel Mesh \"Fregament\" Shader: Initilized... ");
	shader->loadGeometryShader("Shaders/shader.geom");
	printf("Voxel Mesh \"Geometery\" Shader: Initilized... ");
	shader->loadVertexShader("Shaders/shader.vert");
	printf("Voxel Mesh \"Vertex\" Shader: Initilized... \n");
	shader->link();
	printf("Voxel Mesh Shader: Linked... \n");

	pMatrixLocation = glGetUniformLocation(shader->id(), "p");
	mMatrixLocation = glGetUniformLocation(shader->id(), "m");
	vMatrixLocation = glGetUniformLocation(shader->id(), "v");

	printf("Scene set up \n");
}

inline double diffclock(clock_t clock1, clock_t clock2)
{
	double diffticks = clock1 - clock2;
	double diffms = diffticks / (CLOCKS_PER_SEC / 1000);

	return diffms;
}

void StereoRender::Draw(GridModel* model, KinectTool* tool, glm::mat4& view,
	glm::mat4& obj, TextureMappedFont* font1, TextureMappedFont* font2,
	TextureMappedFont* font3, bool left)
{
	clock_t start = clock();

	glm::mat4 projectionMatrix = (left)? projectionMatrixLeft : projectionMatrixRight;

	// Specifies whether the depth buffer is enabled for writing.
	glDepthMask(GL_TRUE);

	// glClear sets the bitplane area of the window to values previously selected 
	// by glClearColor, glClearDepth, and glClearStencil.
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// GL_CULL_FACE is to be enabled for performance reasons, 
	// as it easily removes half of the triangles to draw, 
	// normally without visual artifacts if your geometry is watertight, and CCW.
	glEnable(GL_CULL_FACE);
	// save the gpu from using unneccesary alpha calculations
	glDisable(GL_BLEND);

	///////////////////////////   START OF ROOM DRAWTING   ///////////////////////////////////



	///////////////////////////   END OF ROOM DRAWTING   ///////////////////////////////////

	///////////////////////////////   START OF MESH DRAWTING   ///////////////////////////////////
	/* Use mesh shader */
	shader->bind();

	// glUniform -	specify the value of a uniform variable for the current program object
	// void glUniformMatrix4fv(	GLint location, GLsizei count, 
	//							GLboolean transpose, const GLfloat *value);
	// location  -	Specifies the location of the uniform value to be modified.
	// count	 -	Specifies the number of matrices that are to be modified. 
	//				This should be 1 if the targeted uniform variable is not 
	//				an array of matrices, and 1 or more if it is an array of matrices.
	// transpose -  Specifies whether to transpose the matrix as the values are 
	//				loaded into the uniform variable. Must be GL_FALSE.
	// value	 -	Specifies a pointer to an array of count values that will be 
	//				used to update the specified uniform variable.

	glUniformMatrix4fv(pMatrixLocation, 1, GL_FALSE, &(projectionMatrix[0][0]));
	glUniformMatrix4fv(mMatrixLocation, 1, GL_FALSE, &(obj[0][0]));
	glUniformMatrix4fv(vMatrixLocation, 1, GL_FALSE, &(view[0][0]));

	/* Gets redndeble Cells */
	std::map< unsigned int, VAO* >* cells = model->GetRenderableCells();
	/* temp interator variabel for going through rendebel cells */
	std::map< unsigned int, VAO* >::iterator iter;

	// New Empty Vertex Array Output
	VAO* vao_ptr = NULL;

	/* ??????? */
	int i = 0;

	// Looks at every renderbal cell and it it is not the end sell it continues 
	// End then it writes them to the skreen.
	for (iter = cells->begin(); iter != cells->end(); iter++)
	{
		vao_ptr = iter->second;
		// glBindVertexArray binds the vertex array object with name array. 
		// array is the name of a vertex array object previously returned 
		// from a call to glGenVertexArrays, or zero to break the existing 
		// vertex array object binding. 
		// If no vertex array object with name array exists, one is created 
		// when array is first bound. If the bind is successful no change 
		// is made to the state of the vertex array object, and any previous 
		// vertex array object binding is broken.
		glBindVertexArray(vao_ptr->id());

		glDrawElements(
			GL_POINTS,			// mode
			vao_ptr->size(),   // count
			GL_UNSIGNED_INT,   // type
			(void*)0           // element array buffer offset
			);
		i++;
	}

	glBindVertexArray(0);
	// We are basikly unbinding the vertex array

	shader->unbind();
	/* Do not use mesh shader */

	glm::mat4 pvm = projectionMatrix*view;

	////////////////////////////////   END OF MESH DRAWTING   ///////////////////////////////////

	glDisable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glDepthMask(GL_FALSE);


	///////////////////////////   START OF KINECT TOOL DRAWTING   ///////////////////////////////////

	// Gets tringelMesh Vertex Array Output
	vao_ptr = tool->GetToolMesh()->GetVAO();

	/* Use tool shader */
	tool->GetToolShader()->bind();

	// glBindVertexArray binds the vertex array object with name array. 
	// array is the name of a vertex array object previously returned 
	// from a call to glGenVertexArrays, or zero to break the existing 
	// vertex array object binding. 

	glUniformMatrix4fv(tool->GetPVMLocation(), 1, GL_FALSE, &(pvm[0][0]));

	glBindVertexArray(vao_ptr->id());

	glDrawElements(
		GL_TRIANGLES,		// mode
		vao_ptr->size(),		// count
		GL_UNSIGNED_INT,	// type
		(void*)0			// element array buffer offset
		);
	glBindVertexArray(0);

	tool->GetToolShader()->unbind();
	/* Do not use tool shader */

	///////////////////////////   END OF KINECT TOOL DRAWTING   ///////////////////////////////////

	
	//////////////////////////////   START OF TEXT DRAWTING   ////////////////////////////////////////////
	DrawingStages(font1, font2, font3);
	//////////////////////////////   END OF TEXT DRAWTING   //////////////////////////////////////////////

	//////////////////////////   START OF LAST TEXT DRAWTING   ////////////////////////////////////////////
	DrawingBottomText(font3);
	///////////////////////////   END OF LAST TEXT DRAWTING   //////////////////////////////////////////////
	
	/////////////////////////////   START OF LINE DRAWTING   ////////////////////////////////////////////

	//////////////////////////////   END OF LINE DRAWTING   ////////////////////////////////////////////



	clock_t end = clock();
	//std::cout<<"For ticks = "<<i<<", tick time = " << diffclock( end, start )<< " ms" << std::endl;
}

void StereoRender::Resize(int w, int h)
{
	glViewport(0, 0, w, h);
	windowWidth = w;
	windowHeight = h;
	float ratio = (float)w / (float)h;
	SetProjections(glm::perspective(FOV, ratio, ZNEAR, ZFAR), glm::perspective(FOV, ratio, ZNEAR, ZFAR));
}

void StereoRender::SetProjections(glm::mat4& leftEye, glm::mat4& rightEye)
{
	projectionMatrixLeft = leftEye;
	projectionMatrixRight = rightEye;
}

void StereoRender::DrawingStages(TextureMappedFont* font1, TextureMappedFont* font2,
	TextureMappedFont* font3)
{
	if (GetStage() == 1) // START
	{
		back =
			reinitiate =
			spin =
			right = left = up = down = clockwise = counter_clockwise = freez = clear = slower = faster =
			rotate_view =
			clear_view = left_view = bottom_view = back_view =
			right_view = top_view = front_view = freez_view =
			rotate = front = rear = top = bottom = camera_right = camera_left =
			sound_on = sound_off =
			haptics_on = haptics_off =
			false;

		model =
			camera =
			sound =
			haptics =
			true;


		model_acted =
			rotate_acted =
			spin_acted =
			rotate_acted =
			camera_acted =
			camera_rotate_acted =
			sound_acted =
			haptics_acted =
			false;

	}
	else if (GetStage() == 2)	// MODEL
	{
		right = left = up = down = clockwise = counter_clockwise = freez = clear = slower = faster =
			clear_view = left_view = bottom_view = back_view =
			right_view = top_view = front_view = freez_view =
			camera =
			rotate = front = rear = top = bottom = camera_right = camera_left =
			sound =
			sound_on = sound_off =
			haptics =
			haptics_on = haptics_off =
			false;

		back =
			model =
			reinitiate =
			spin =
			rotate_view =
			true;

		spin_acted = rotate_acted = false;

		model_acted = true;
	}
	else if (GetStage() == 3) // SPIN
	{
		reinitiate =
			rotate_view =
			clear_view = left_view = bottom_view = back_view =
			right_view = top_view = front_view = freez_view =
			camera =
			rotate = front = rear = top = bottom = camera_right = camera_left =
			sound =
			sound_on = sound_off =
			haptics =
			haptics_on = haptics_off =
			false;

		back =
			model =
			spin =
			right = left = up = down = clockwise = counter_clockwise = freez = clear = slower = faster =

			true;


		model_acted = spin_acted = true;
	}

	else if (GetStage() == 4) // ROTATE
	{
		model =
			reinitiate =
			spin =
			right = left = up = down = clockwise = counter_clockwise = freez = clear = slower = faster =
			camera =
			rotate = front = rear = top = bottom = camera_right = camera_left =
			sound =
			sound_on = sound_off =
			haptics =
			haptics_on = haptics_off =
			false;

		back =
			model =
			rotate_view =
			clear_view = left_view = bottom_view = back_view =
			right_view = top_view = front_view = freez_view =
			true;

		model_acted = rotate_acted = true;
	}

	else if (GetStage() == 5) // CAMERA
	{
		model =
			reinitiate =
			spin =
			right = left = up = down = clockwise = counter_clockwise = freez = clear = slower = faster =
			rotate_view =
			clear_view = left_view = bottom_view = back_view =
			right_view = top_view = front_view = freez_view =
			sound =
			sound_on = sound_off =
			haptics =
			haptics_on = haptics_off =
			false;

		back =
			camera =
			rotate = front = rear = top = bottom = camera_right = camera_left =
			true;

		camera_rotate_acted = camera_acted = true;
	}
	else if (GetStage() == 6) //SOUND
	{
		model =
			reinitiate =
			spin =
			right = left = up = down = clockwise = counter_clockwise = freez = clear = slower = faster =
			rotate_view =
			clear_view = left_view = bottom_view = back_view =
			right_view = top_view = front_view = freez_view =
			camera =
			rotate = front = rear = top = bottom = camera_right = camera_left =
			haptics =
			haptics_on = haptics_off =
			false;

		back =
			sound =
			sound_on = sound_off =
			true;

		sound_acted = true;
	}
	else if (GetStage() == 7) //HAPTICS
	{

		model =
			reinitiate =
			spin =
			right = left = up = down = clockwise = counter_clockwise = freez = clear = slower = faster =
			rotate_view =
			clear_view = left_view = bottom_view = back_view =
			right_view = top_view = front_view = freez_view =
			camera =
			rotate = front = rear = top = bottom = camera_right = camera_left =
			sound =
			sound_on = sound_off =
			false;

		back =
			haptics =
			haptics_on = haptics_off =
			true;

		haptics_acted = true;
	}
	else
	{
		back =
			model =
			reinitiate =
			spin =
			right = left = up = down = clockwise = counter_clockwise = freez = clear = slower = faster =
			rotate_view =
			clear_view = left_view = bottom_view = back_view =
			right_view = top_view = front_view = freez_view =
			camera =
			rotate = front = rear = top = bottom = camera_right = camera_left =
			sound =
			sound_on = sound_off =
			haptics =
			haptics_on = haptics_off =
			true;
	}

	for (int i = 0; i < 26; i++)
	{
		if (actedIndex[i] < 1.0f)
			actedIndex[i] += 0.01f;
		else
		{
			switch (i)
			{
			case 0: SetBackStage(false);			break;
			case 1: SetReinitiateStage(false);		break;
			case 2: SetRightStage(false);			break;
			case 3: SetLeftStage(false);			break;
			case 4: SetUpStage(false);				break;
			case 5: SetDownStage(false);			break;
			case 6: SetClockwiseStage(false);		break;
			case 7: SetCounterClockwiseStage(false); break;
			case 8: SetFreezStage(false);			break;
			case 9: SetClearStage(false);			break;
			case 10: SetFrontViewStage(false);		break;
			case 11: SetRearViewStage(false);		break;
			case 12: SetTopViewStage(false);		break;
			case 13: SetBottomViewStage(false);		break;
			case 14: SetRightViewStage(false);		break;
			case 15: SetLeftViewStage(false);		break;
			case 16: SetFreezViewStage(false);		break;
			case 17: SetclearViewStage(false);		break;
			case 18: SetFrontStage(false);			break;
			case 19: SetRearStage(false);			break;
			case 20: SetTopStage(false);			break;
			case 21: SetBottomStage(false);			break;
			case 22: SetCameraRightStage(false);	break;
			case 23: SetCameraLeftStage(false);		break;
			case 24: SetFasterStage(false);			break;
			case 25: SetSlowerStage(false);			break;
			}
		}
	}
	int font3xp = 1540;
	int font2xp = font3xp + 40;
	int font1xp = font2xp + 20;
	int font0xp = font1xp + 20;

	int fontTopYP = 1000;

	int positionY[40];
	int stepLevel1 = 60;
	int stepLevel2 = 40;
	int stepLevel3 = 20;
	if (speechON)
	{
		if (viewDrawStage)
		{
			if (GetStage() == 1)
			{
				int currentPosition = fontTopYP;
				font2->DrawString(font2xp, currentPosition -= stepLevel1, "START SCULPTING", true, GetPressedStage(), false, 1);
				font2->DrawString(font2xp, currentPosition -= stepLevel2, "END SCULPTING", true, !GetPressedStage(), false, 1);
				font2->DrawString(font2xp, currentPosition -= stepLevel2, "MODEL", model, model_acted, false, 1);
				font2->DrawString(font2xp, currentPosition -= stepLevel2, "CAMERA", camera, camera_acted, false, 1);

				font2->DrawString(font2xp, currentPosition -= stepLevel2, "SOUND", sound, sound_acted, false, 1);
				if (soundON)
					font2->DrawString(font2xp + 120, currentPosition, "(ON)", sound, true, false, 1);
				else
					font2->DrawString(font2xp + 120, currentPosition, "(OFF)", sound, true, false, 1);

				font2->DrawString(font2xp, currentPosition -= stepLevel2, "HAPTICS", haptics, haptics_acted, false, 1);
				if (hapticsON)
					font2->DrawString(font2xp + 160, currentPosition, "(ON)", sound, true, false, 1);
				else
					font2->DrawString(font2xp + 160, currentPosition, "(OFF)", sound, true, false, 1);


				font2->DrawString(font2xp, currentPosition -= stepLevel2, "QUIT PROGRAM", true, false, false, 1);
			}
			else if (GetStage() == 2)
			{
				int currentPosition = fontTopYP;
				font3->DrawString(font3xp, fontTopYP, "CLEAR SELECTION", back, false, GetBackStage(), actedIndex[0]);
				font2->DrawString(font2xp, currentPosition -= stepLevel1, "START SCULPTING", true, GetPressedStage(), false, 1);
				font2->DrawString(font2xp, currentPosition -= stepLevel2, "END SCULPTING", true, !GetPressedStage(), false, 1);
				font2->DrawString(font2xp, currentPosition -= stepLevel2, "MODEL", model, model_acted, false, 1);
				font1->DrawString(font1xp, currentPosition -= stepLevel3, "REINITIATE", reinitiate, false, GetReinitiateStage(), actedIndex[1]); //
				font1->DrawString(font1xp, currentPosition -= stepLevel3, "SPIN", spin, spin_acted, false, 1);
				font1->DrawString(font1xp, currentPosition -= stepLevel3, "ROTATE", rotate_view, rotate_acted, false, 1);
				font2->DrawString(font2xp, currentPosition -= stepLevel2, "QUIT PROGRAM", true, false, false, 1);
			}
			else if (GetStage() == 3)
			{
				int currentPosition = fontTopYP;
				font3->DrawString(font3xp, fontTopYP, "CLEAR SELECTION", back, false, GetBackStage(), actedIndex[0]);

				font2->DrawString(font2xp, currentPosition -= stepLevel1, "START SCULPTING", true, GetPressedStage(), false, 1);
				font2->DrawString(font2xp, currentPosition -= stepLevel2, "END SCULPTING", true, !GetPressedStage(), false, 1);

				font2->DrawString(font2xp, currentPosition -= stepLevel2, "MODEL", model, model_acted, false, 1);
				font1->DrawString(font1xp, currentPosition -= stepLevel3, "SPIN", spin, spin_acted, false, 1);
				font1->DrawString(font0xp, currentPosition -= stepLevel3, "RIGHT", right, false, GetRightStage(), actedIndex[2]);
				font1->DrawString(font0xp, currentPosition -= stepLevel3, "LEFT", left, false, GetLeftStage(), actedIndex[3]);
				font1->DrawString(font0xp, currentPosition -= stepLevel3, "UP", up, false, GetUpStage(), actedIndex[4]);
				font1->DrawString(font0xp, currentPosition -= stepLevel3, "DOWN", down, false, GetDownStage(), actedIndex[5]);
				font1->DrawString(font0xp, currentPosition -= stepLevel3, "CLOCKWISE", clockwise, false, GetClockwiseStage(), actedIndex[6]);
				font1->DrawString(font0xp, currentPosition -= stepLevel3, "COUNTER CLOCKWISE", counter_clockwise, false, GetCounterClockwiseStage(), actedIndex[7]);
				font1->DrawString(font0xp, currentPosition -= stepLevel3, "FASTER", faster, false, GetFasterStage(), actedIndex[24]);
				font1->DrawString(font0xp, currentPosition -= stepLevel3, "SLOWER", slower, false, GetSlowerStage(), actedIndex[25]);
				font1->DrawString(font0xp, currentPosition -= stepLevel3, "FREEZE", freez, false, GetFreezStage(), actedIndex[8]);
				font1->DrawString(font0xp, currentPosition -= stepLevel3, "CLEAR", clear, false, GetClearStage(), actedIndex[9]);

				font2->DrawString(font2xp, currentPosition -= stepLevel2, "QUIT PROGRAM", true, false, false, 1);
			}
			else if (GetStage() == 4)
			{
				int currentPosition = fontTopYP;
				font3->DrawString(font3xp, fontTopYP, "CLEAR SELECTION", back, false, GetBackStage(), actedIndex[0]);

				font2->DrawString(font2xp, currentPosition -= stepLevel1, "START SCULPTING", true, GetPressedStage(), false, 1);
				font2->DrawString(font2xp, currentPosition -= stepLevel2, "END SCULPTING", true, !GetPressedStage(), false, 1);

				font2->DrawString(font2xp, currentPosition -= stepLevel2, "MODEL", model, model_acted, false, 1);

				font1->DrawString(font1xp, currentPosition -= stepLevel3, "ROTATE", rotate_view, rotate_acted, false, 1);
				font1->DrawString(font0xp, currentPosition -= stepLevel3, "FRONT", front_view, false, GetFrontViewStage(), actedIndex[10]);
				font1->DrawString(font0xp, currentPosition -= stepLevel3, "REAR", back_view, false, GetRearViewStage(), actedIndex[11]);
				font1->DrawString(font0xp, currentPosition -= stepLevel3, "TOP", top_view, false, GetTopViewStage(), actedIndex[12]);
				font1->DrawString(font0xp, currentPosition -= stepLevel3, "BOTTOM", bottom_view, false, GetBottomViewStage(), actedIndex[13]);
				font1->DrawString(font0xp, currentPosition -= stepLevel3, "RIGHT", right_view, false, GetRightViewStage(), actedIndex[14]);
				font1->DrawString(font0xp, currentPosition -= stepLevel3, "LEFT", left_view, false, GetLeftViewStage(), actedIndex[15]);
				font1->DrawString(font0xp, currentPosition -= stepLevel3, "FREEZE", freez_view, false, GetFreezViewStage(), actedIndex[16]);
				font1->DrawString(font0xp, currentPosition -= stepLevel3, "CLEAR", clear_view, false, GetclearViewStage(), actedIndex[17]);

				font2->DrawString(font2xp, currentPosition -= stepLevel2, "QUIT PROGRAM", true, false, false, 1);
			}
			else if (GetStage() == 5)
			{
				int currentPosition = fontTopYP;
				font3->DrawString(font3xp, fontTopYP, "CLEAR SELECTION", back, false, GetBackStage(), actedIndex[0]);

				font2->DrawString(font2xp, currentPosition -= stepLevel1, "START SCULPTING", true, GetPressedStage(), false, 1);
				font2->DrawString(font2xp, currentPosition -= stepLevel2, "END SCULPTING", true, !GetPressedStage(), false, 1);

				font2->DrawString(font2xp, currentPosition -= stepLevel2, "CAMERA", camera, camera_acted, false, 1);
				font1->DrawString(font1xp, currentPosition -= stepLevel3, "ROTATE", rotate, camera_rotate_acted, false, 1);
				font1->DrawString(font0xp, currentPosition -= stepLevel3, "FRONT", front, false, GetFrontStage(), actedIndex[18]); //
				font1->DrawString(font0xp, currentPosition -= stepLevel3, "REAR", rear, false, GetRearStage(), actedIndex[19]); //
				font1->DrawString(font0xp, currentPosition -= stepLevel3, "TOP", top, false, GetTopStage(), actedIndex[20]); //
				font1->DrawString(font0xp, currentPosition -= stepLevel3, "BOTTOM", bottom, false, GetBottomStage(), actedIndex[21]); //
				font1->DrawString(font0xp, currentPosition -= stepLevel3, "RIGHT", camera_right, false, GetCameraRightStage(), actedIndex[22]); //
				font1->DrawString(font0xp, currentPosition -= stepLevel3, "LEFT", camera_left, false, GetCameraLeftStage(), actedIndex[23]); //

				font2->DrawString(font2xp, currentPosition -= stepLevel2, "QUIT PROGRAM", true, false, false, 1);
			}
			else if (GetStage() == 6)
			{
				int currentPosition = fontTopYP;
				font3->DrawString(font3xp, fontTopYP, "CLEAR SELECTION", back, false, GetBackStage(), actedIndex[0]);

				font2->DrawString(font2xp, currentPosition -= stepLevel1, "START SCULPTING", true, GetPressedStage(), false, 1);
				font2->DrawString(font2xp, currentPosition -= stepLevel2, "END SCULPTING", true, !GetPressedStage(), false, 1);

				font2->DrawString(font2xp, currentPosition -= stepLevel2, "SOUND", sound, sound_acted, false, 1);

				if (soundON)
					font2->DrawString(font2xp + 120, currentPosition, "(ON)", sound, true, false, 1);
				else
					font2->DrawString(font2xp + 120, currentPosition, "(OFF)", sound, true, false, 1);

				font1->DrawString(font1xp, currentPosition -= stepLevel3, "TURN ON", sound_on, GetSoundStage(), false, 1);
				font1->DrawString(font1xp, currentPosition -= stepLevel3, "TURN OFF", sound_on, !GetSoundStage(), false, 1);

				font2->DrawString(font2xp, currentPosition -= stepLevel2, "QUIT PROGRAM", true, false, false, 1);
			}
			else if (GetStage() == 7)
			{
				int currentPosition = fontTopYP;
				font3->DrawString(font3xp, fontTopYP, "CLEAR SELECTION", back, false, GetBackStage(), actedIndex[0]);

				font2->DrawString(font2xp, currentPosition -= stepLevel1, "START SCULPTING", true, GetPressedStage(), false, 1);
				font2->DrawString(font2xp, currentPosition -= stepLevel2, "END SCULPTING", true, !GetPressedStage(), false, 1);

				font2->DrawString(font2xp, currentPosition -= stepLevel2, "HAPTICS", haptics, haptics_acted, false, 1);

				if (hapticsON)
					font2->DrawString(font2xp + 160, currentPosition, "(ON)", sound, true, false, 1);
				else
					font2->DrawString(font2xp + 160, currentPosition, "(OFF)", sound, true, false, 1);

				font1->DrawString(font1xp, currentPosition -= stepLevel3, "TURN ON", haptics_on, GetHapticsStage(), false, 1);
				font1->DrawString(font1xp, currentPosition -= stepLevel3, "TURN OFF", haptics_off, !GetHapticsStage(), false, 1);

				font2->DrawString(font2xp, currentPosition -= stepLevel2, "QUIT PROGRAM", true, false, false, 1);
			}
		}
		else
		{
			font3->DrawString(font3xp, fontTopYP, "CLEAR SELECTION", back, false, GetBackStage(), actedIndex[0]); //

			font2->DrawString(font2xp, fontTopYP - 60, "START SCULPTING", true, GetPressedStage(), false, 1);

			font2->DrawString(font2xp, fontTopYP - 100, "END SCULPTING", true, !GetPressedStage(), false, 1);

			font2->DrawString(font2xp, fontTopYP - 140, "MODEL", model, model_acted, false, 1);
			font1->DrawString(font1xp, fontTopYP - 160, "REINITIATE", reinitiate, false, GetReinitiateStage(), actedIndex[1]); //
			font1->DrawString(font1xp, fontTopYP - 180, "SPIN", spin, spin_acted, false, 1);
			font1->DrawString(font0xp, fontTopYP - 200, "RIGHT", right, false, GetRightStage(), actedIndex[2]); //
			font1->DrawString(font0xp, fontTopYP - 220, "LEFT", left, false, GetLeftStage(), actedIndex[3]); //
			font1->DrawString(font0xp, fontTopYP - 240, "UP", up, false, GetUpStage(), actedIndex[4]); //
			font1->DrawString(font0xp, fontTopYP - 260, "DOWN", down, false, GetDownStage(), actedIndex[5]); //
			font1->DrawString(font0xp, fontTopYP - 280, "CLOCKWISE", clockwise, false, GetClockwiseStage(), actedIndex[6]); //
			font1->DrawString(font0xp, fontTopYP - 300, "COUNTER CLOCKWISE", counter_clockwise, false, GetCounterClockwiseStage(), actedIndex[7]); //

			font1->DrawString(font0xp, fontTopYP - 320, "FASTER", faster, false, GetFasterStage(), actedIndex[24]); //
			font1->DrawString(font0xp, fontTopYP - 340, "SLOWER", slower, false, GetSlowerStage(), actedIndex[25]); //

			font1->DrawString(font0xp, fontTopYP - 360, "FREEZE", freez, false, GetFreezStage(), actedIndex[8]); //
			font1->DrawString(font0xp, fontTopYP - 380, "CLEAR", clear, false, GetClearStage(), actedIndex[9]); //

			font1->DrawString(font1xp, fontTopYP - 400, "ROTATE", rotate_view, rotate_acted, false, 1);
			font1->DrawString(font0xp, fontTopYP - 420, "FRONT", front_view, false, GetFrontViewStage(), actedIndex[10]); //
			font1->DrawString(font0xp, fontTopYP - 440, "REAR", back_view, false, GetRearViewStage(), actedIndex[11]); //
			font1->DrawString(font0xp, fontTopYP - 460, "TOP", top_view, false, GetTopViewStage(), actedIndex[12]); //
			font1->DrawString(font0xp, fontTopYP - 480, "BOTTOM", bottom_view, false, GetBottomViewStage(), actedIndex[13]); //
			font1->DrawString(font0xp, fontTopYP - 500, "RIGHT", right_view, false, GetRightViewStage(), actedIndex[14]); //
			font1->DrawString(font0xp, fontTopYP - 520, "LEFT", left_view, false, GetLeftViewStage(), actedIndex[15]); //
			font1->DrawString(font0xp, fontTopYP - 540, "FREEZE", freez_view, false, GetFreezViewStage(), actedIndex[16]); //
			font1->DrawString(font0xp, fontTopYP - 560, "CLEAR", clear_view, false, GetclearViewStage(), actedIndex[17]); //

			font2->DrawString(font2xp, fontTopYP - 600, "CAMERA", camera, camera_acted, false, 1);
			font1->DrawString(font1xp, fontTopYP - 620, "ROTATE", rotate, camera_rotate_acted, false, 1);
			font1->DrawString(font0xp, fontTopYP - 640, "FRONT", front, false, GetFrontStage(), actedIndex[18]); //
			font1->DrawString(font0xp, fontTopYP - 660, "REAR", rear, false, GetRearStage(), actedIndex[19]); //
			font1->DrawString(font0xp, fontTopYP - 680, "TOP", top, false, GetTopStage(), actedIndex[20]); //
			font1->DrawString(font0xp, fontTopYP - 700, "BOTTOM", bottom, false, GetBottomStage(), actedIndex[21]); //
			font1->DrawString(font0xp, fontTopYP - 720, "RIGHT", camera_right, false, GetCameraRightStage(), actedIndex[22]); //
			font1->DrawString(font0xp, fontTopYP - 740, "LEFT", camera_left, false, GetCameraLeftStage(), actedIndex[23]); //

			font2->DrawString(font2xp, fontTopYP - 800, "SOUND", sound, sound_acted, false, 1);
			font1->DrawString(font1xp, fontTopYP - 820, "TURN ON", sound_on, GetSoundStage(), false, 1);
			font1->DrawString(font1xp, fontTopYP - 840, "TURN OFF", sound_on, !GetSoundStage(), false, 1);

			font2->DrawString(font2xp, fontTopYP - 900, "HAPTICS", haptics, haptics_acted, false, 1);
			font1->DrawString(font1xp, fontTopYP - 920, "TURN ON", haptics_on, GetHapticsStage(), false, 1);
			font1->DrawString(font1xp, fontTopYP - 940, "TURN OFF", haptics_off, !GetHapticsStage(), false, 1);

			font2->DrawString(font2xp, fontTopYP - 1000, "QUIT PROGRAM", true, false, false, 1);
		}
		model_acted, spin_acted, camera_acted, rotate_acted, camera_rotate_acted, sound_acted, haptics_acted = false;
	}
}

void StereoRender::DrawingBottomText(TextureMappedFont* font3)
{

	for (int i = 0; i < 26; i++)
	{
		if (actedIndex[i] < 1.0f)
			actedIndex[i] += 0.01f;
		else
		{
			if (i == 0)
				SetBackStage(false);
		}
	}

	if (lastWord == 1)//
		font3->DrawString(20, 20, "CLEAR SELECTION", GetBackStage(), true, GetBackStage(), actedIndex[0]);
	else if (lastWord == 2)//
		font3->DrawString(20, 20, "START SCULPTING", true, true, false, 1);
	else if (lastWord == 3)//
		font3->DrawString(20, 20, "END SCULPTING", true, true, false, 1);
	else if (lastWord == 4)
		font3->DrawString(20, 20, "MODEL", true, true, false, 1);
	else if (lastWord == 5)
		font3->DrawString(20, 20, "REINITATE", GetReinitiateStage(), true, GetReinitiateStage(), actedIndex[1]);
	else if (lastWord == 6)
		font3->DrawString(20, 20, "SPIN", true, true, false, 1);

	else if (lastWord == 19)
		font3->DrawString(20, 20, "RIGHT", GetRightStage(), true, GetRightStage(), actedIndex[2]);
	else if (lastWord == 7)
		font3->DrawString(20, 20, "LEFT", GetLeftStage(), true, GetLeftStage(), actedIndex[3]);

	else if (lastWord == 8)
		font3->DrawString(20, 20, "UP", GetUpStage(), true, GetUpStage(), actedIndex[4]);
	else if (lastWord == 9)
		font3->DrawString(20, 20, "DOWN", GetDownStage(), true, GetDownStage(), actedIndex[5]);
	else if (lastWord == 10)
		font3->DrawString(20, 20, "CLOCKWISE", GetClockwiseStage(), true, GetClockwiseStage(), actedIndex[6]);
	else if (lastWord == 11)
		font3->DrawString(20, 20, "COUNTER CLOCKWISE", GetCounterClockwiseStage(), true, GetCounterClockwiseStage(), actedIndex[7]);
	else if (lastWord == 12)
		font3->DrawString(20, 20, "FREEZE", GetFreezStage(), true, GetFreezStage(), actedIndex[8]);
	else if (lastWord == 13)
		font3->DrawString(20, 20, "CLEAR", GetClearStage(), true, GetClearStage(), actedIndex[9]);
	else if (lastWord == 14)
		font3->DrawString(20, 20, "ROTATE", true, true, false, 1);
	else if (lastWord == 15)
		font3->DrawString(20, 20, "FRONT", GetFrontViewStage(), true, GetFrontViewStage(), actedIndex[10]);
	else if (lastWord == 16)
		font3->DrawString(20, 20, "REAR", GetRearViewStage(), true, GetRearViewStage(), actedIndex[11]);
	else if (lastWord == 17)
		font3->DrawString(20, 20, "TOP", GetTopViewStage(), true, GetTopViewStage(), actedIndex[12]);
	else if (lastWord == 18)
		font3->DrawString(20, 20, "BOTTOM", GetBottomViewStage(), true, GetBottomViewStage(), actedIndex[13]);



	else if (lastWord == 20)
		font3->DrawString(20, 20, "CAMERA", true, true, false, 1);
	else if (lastWord == 21)
		font3->DrawString(20, 20, "SOUND", true, true, false, 1);
	else if (lastWord == 22)
		font3->DrawString(20, 20, "TURN ON", true, true, false, 1);
	else if (lastWord == 23)
		font3->DrawString(20, 20, "TURN OFF", true, true, false, 1);
	else if (lastWord == 24)
		font3->DrawString(20, 20, "HAPTICS", true, true, false, 1);
	else if (lastWord == 25)
		font3->DrawString(20, 20, "QUIT PROGRAM", true, true, false, 1);


	else if (lastWord == 26)
		font3->DrawString(20, 20, "RIGHT", GetRightViewStage(), true, GetRightViewStage(), actedIndex[14]);
	else if (lastWord == 27)
		font3->DrawString(20, 20, "LEFT", GetLeftViewStage(), true, GetLeftViewStage(), actedIndex[15]);



	else if (lastWord == 28)
		font3->DrawString(20, 20, "FREEZE", GetFreezViewStage(), true, GetFreezViewStage(), actedIndex[16]);
	else if (lastWord == 29)
		font3->DrawString(20, 20, "CLEAR", GetclearViewStage(), true, GetclearViewStage(), actedIndex[17]);


	else if (lastWord == 30)
		font3->DrawString(20, 20, "FRONT", GetFrontStage(), true, GetFrontStage(), actedIndex[18]);
	else if (lastWord == 31)
		font3->DrawString(20, 20, "REAR", GetRearStage(), true, GetRearStage(), actedIndex[19]);
	else if (lastWord == 32)
		font3->DrawString(20, 20, "TOP", GetTopStage(), true, GetTopStage(), actedIndex[20]);
	else if (lastWord == 33)
		font3->DrawString(20, 20, "BOTTOM", GetBottomStage(), true, GetBottomStage(), actedIndex[21]);

	else if (lastWord == 34)
		font3->DrawString(20, 20, "RIGHT", GetCameraRightStage(), true, GetCameraRightStage(), actedIndex[22]);
	else if (lastWord == 35)
		font3->DrawString(20, 20, "LEFT", GetCameraLeftStage(), true, GetCameraLeftStage(), actedIndex[23]);

	else if (lastWord == 36)
		font3->DrawString(20, 20, "FASTER", GetFasterStage(), true, GetFasterStage(), actedIndex[24]);
	else if (lastWord == 37)
		font3->DrawString(20, 20, "SLOWER", GetSlowerStage(), true, GetSlowerStage(), actedIndex[25]);


	else
		font3->DrawString(20, 20, "", true, true, false, 1);

}