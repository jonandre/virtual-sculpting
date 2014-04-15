#ifndef STEREO_RENDER_H
#define STEREO_RENDER_H

#include "Shader.h"
#include "main.h"

#include "GraphicsLib.h"
#include "GridModel.h"
#include "VAO.h"
#include "KinectTool.h"
#include "TriangleMesh.h"
#include <time.h>

#include "texturemappedfont.h"
#include "Stage.h"

class StereoRender
{
public:
	static const float FOV;
	static const float ZNEAR;
	static const float ZFAR;

	Shader* shader;
	StereoRender();
	~StereoRender();
	void Init();
	void SetupScene();
	void Resize(int w, int h);
	void Draw(GridModel* model, KinectTool* _tool_mesh, glm::mat4& view,
		glm::mat4& obj, TextureMappedFont*, TextureMappedFont*,
		TextureMappedFont*, glm::mat4& proj, int status);

	void SetProjections(glm::mat4& leftEye, glm::mat4& rightEye);

private:
	glm::mat4 projectionMatrixLeft;
	glm::mat4 projectionMatrixRight;

	int windowWidth;
	int windowHeight;

	int pMatrixLocation;
	int vMatrixLocation;
	int mMatrixLocation;

	void DrawingStages(TextureMappedFont*, TextureMappedFont*,
		TextureMappedFont*);

	bool	back,
		model,
		reinitiate,
		spin,
		right, left, up, down, clockwise, counter_clockwise, freez, clear, faster, slower,
		rotate_view,
		clear_view, left_view, bottom_view, back_view,
		right_view, top_view, front_view, freez_view,
		camera,
		rotate, front, rear, top, bottom, camera_right, camera_left,
		sound,
		sound_on, sound_off,
		haptics,
		haptics_on, haptics_off;


	bool model_acted, spin_acted, camera_acted, rotate_acted, sound_acted,
		haptics_acted, camera_rotate_acted;


	void DrawingBottomText(TextureMappedFont* font3);
};

#endif // STEREO_RENDER_H
