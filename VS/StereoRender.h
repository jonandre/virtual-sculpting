class GridModel;
#include "Shader.h"
#include "main.h"

#include "texturemappedfont.h"

class KinectTool;

class StereoRender
{
public:
	const float FOV = 30.0f;
	const float ZNEAR = 0.1f;
	const float ZFAR = 4048.f;

	Shader* shader;
	StereoRender();
	~StereoRender();
	void Init();
	void SetupScene();
	void Resize(int w, int h);
	void Draw(GridModel* model, KinectTool* _tool_mesh, glm::mat4& view,
		glm::mat4& obj, TextureMappedFont*, TextureMappedFont*,
		TextureMappedFont*, bool left);

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
