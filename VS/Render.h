class GridModel;
#include "Shader.h"
#include "main.h"

#include "texturemappedfont.h"

class KinectTool;

class Render
{
public:
	Shader* shader;
	Render();
	~Render();
	void Init();
	void SetupScene();
	void Resize(int w, int h);
	void Draw( GridModel* model, KinectTool* _tool_mesh, glm::mat4& view, 
				glm::mat4& obj, TextureMappedFont*, TextureMappedFont*,
				TextureMappedFont* );
private:
	glm::mat4 projectionMatrix;

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
		

	bool model_acted,	spin_acted,		camera_acted,	rotate_acted,	sound_acted,
		 haptics_acted, camera_rotate_acted;

	
	void DrawingBottomText( TextureMappedFont* font3);
};
