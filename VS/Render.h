class GridModel;
#include "Shader.h"
#include "main.h"
#include "VSText.h"

class KinectTool;

class Render
{
public:
	Render();
	~Render();
	void Init();
	void SetupScene();
	void Resize(int, int);
	void Draw( GridModel* model, KinectTool* _tool_mesh, glm::mat4& view, glm::mat4& obj );
	VSText* text;


private:
	glm::mat4 projectionMatrix;
	
	int windowWidth;
	int windowHeight;

	Shader *shader;
	int pMatrixLocation;
	int vMatrixLocation;
	int mMatrixLocation;
};

