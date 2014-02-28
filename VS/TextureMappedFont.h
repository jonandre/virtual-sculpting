#pragma once

#include <string>
#include "load_bmp.h"
#include "mat_matr.h"

#include "glslshader.h"

using namespace std;

class TextureMappedFont
{
public:
	TextureMappedFont(void);	
	TextureMappedFont(const string& fontTexture, float fontSize=16.0f);
	~TextureMappedFont(void);

    bool Init();
    void DrawString(float x, float y, const string& string, bool, 
						bool, bool, float);

private:
	
    GLSLShader _font_shader;

	int screen_width;
	int screen_height;

    GLuint _textureID; 
	GLuint _vaoID; 
    
    GLuint _texCoordBuffer;
    GLuint _vertexBuffer;
    float _fontSize;  

	TextureMappedFont* font1;

	matrix_4x4_type P; //orthographic projection matrix
};