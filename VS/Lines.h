#pragma once
#include "mat_matr.h"

#include "glslshader.h"
class Lines
{
public:
	Lines(void);
	~Lines(void);
	void DrawLines(void);

    GLSLShader _lines_shader;
};

