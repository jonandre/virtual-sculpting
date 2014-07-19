#include "TextureMappedFont.h"
#include <cassert>
#include <gl/glew.h>
#include <gl/freeglut.h>
#include <iostream>

#include "VBO.h"	//NEW
#include "VAO.h"	//NEW

#define GL_CHECK_ERRORS assert(glGetError()== GL_NO_ERROR);

TextureMappedFont::TextureMappedFont(void)
{
	printf("No texture was applied and or no font size. \n");
	_textureID = -1; 
	_fontSize = 16; 
}


TextureMappedFont::TextureMappedFont(const string& textureName, float fontSize)
{  
	_textureID = LoadBMP(const_cast<char*>(textureName.c_str())); 


	_font_shader.LoadFromFile(GL_VERTEX_SHADER,"font.vert");
	_font_shader.LoadFromFile(GL_FRAGMENT_SHADER,"font.frag");
	_font_shader.CreateAndLinkProgram();


	GL_CHECK_ERRORS
	_font_shader.Use();	
		_font_shader.AddAttribute("vVertex");
		_font_shader.AddAttribute("vTexCoord");
		_font_shader.AddUniform("texture0");
		_font_shader.AddUniform("selected");
		_font_shader.AddUniform("active");
		_font_shader.AddUniform("temp");
		_font_shader.AddUniform("flo");
		glUniform1i(_font_shader("texture0"),0);
		glUniform1i(_font_shader("selected"),0);
		glUniform1i(_font_shader("active"),0);
		glUniform1i(_font_shader("temp"),0);
		glUniform1f(_font_shader("flo"),0);
	_font_shader.UnUse();

    _fontSize = fontSize;

	Init();
}

TextureMappedFont::~TextureMappedFont(void)
{
	printf("Removied font. \n");
	glDeleteVertexArrays(1, &_vaoID);
	glDeleteBuffers(1, &_vertexBuffer);
	glDeleteBuffers(1, &_texCoordBuffer);
	glDeleteTextures(1, &_textureID);
}

bool TextureMappedFont::Init()
{
	screen_width = 1920;
	screen_height = 1080;

    float vertices [] = {
        0.0f, 0.0f,
        _fontSize, 0.0f, 
        _fontSize, _fontSize,
        0.0f, _fontSize
    };

	GL_CHECK_ERRORS

	glGenVertexArrays(1, &_vaoID);
	glBindVertexArray(_vaoID);
    glGenBuffers(1, &_vertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, _vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 8, &vertices[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(_font_shader["vVertex"]);
    glVertexAttribPointer(_font_shader["vVertex"], 2, GL_FLOAT, GL_FALSE, 0, 0);


	GL_CHECK_ERRORS

    //Just initialize with something for now, the tex coords are updated
    //for each character printed
    float texCoords [] = {
        0.0f, 0.0f,
        0.0f, 0.0f, 
        0.0f, 0.0f,
        0.0f, 0.0f
    };

    glGenBuffers(1, &_texCoordBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, _texCoordBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 8, &texCoords[0], GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(_font_shader["vTexCoord"]);
    glVertexAttribPointer(_font_shader["vTexCoord"], 2, GL_FLOAT, GL_FALSE, 0, 0);

	GL_CHECK_ERRORS
	//set the orthographic projection matrix
	Orthographic(0.f, float(screen_width), 0.f , float(screen_height),-1.f,1.f, P);
    return true;
}



void TextureMappedFont::DrawString(float x, float y, const string& str,
								   bool sel, bool act, bool temp, float flo) 
{
    //static float modelviewMatrix[32];
    //static float projectionMatrix[32];

	GL_CHECK_ERRORS
	
    GLint test[1];
	glGetIntegerv(GL_DEPTH_TEST, test);
    glDisable(GL_DEPTH_TEST);
	 
    _font_shader.Use();
    
    float texCoords[8];

    glBindTexture(GL_TEXTURE_2D, _textureID);
 
	GL_CHECK_ERRORS

	matrix_4x4_type translate,I, MV, oldMV, MVP;
	MatrIdentity_4x4(MV);
	MatrIdentity_4x4(MVP);
	MatrIdentity_4x4(translate);
	MatrIdentity_4x4(I);

	//Position our text
	translate[3][0]=x;
	translate[3][1]=y;

	MatrMul_4x4_4x4(translate, I, MV);
	glBindVertexArray(_vaoID);

    //glTranslatef(x, y, 0.0); //Position our text

    for(string::size_type i = 0; i < str.size(); ++i) 
    {
		MatrMul_4x4_4x4(MV, P, MVP);
        const float oneOverSixteen = 1.0f / 16.0f;

        int ch = int(str[i]);
        float xPos = float(ch % 16) * oneOverSixteen;
        float yPos = float(ch / 16) * oneOverSixteen;

        texCoords[0] = xPos;
        texCoords[1] = 1.0f - yPos - oneOverSixteen;

        texCoords[2] = xPos + oneOverSixteen;
        texCoords[3] = 1.0f - yPos - oneOverSixteen;

        texCoords[4] = xPos + oneOverSixteen;
        texCoords[5] = 1.0f - yPos - 0.001f;

        texCoords[6] = xPos;
        texCoords[7] = 1.0f - yPos - 0.001f;

        glBindBuffer(GL_ARRAY_BUFFER, _texCoordBuffer);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float) * 8, &texCoords[0]);

       	glUniformMatrix4fv(_font_shader("MVP"), 1, GL_FALSE, &MVP[0][0]); 
		glUniform1i(_font_shader("selected"),		sel);
		glUniform1i(_font_shader("active"),			act);
		glUniform1i(_font_shader("temp"),	temp);
		glUniform1f(_font_shader("flo"),	flo);

		GL_CHECK_ERRORS

        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
        
		GL_CHECK_ERRORS

		MatrCopy_4x4(oldMV,MV);
		matrix_4x4_type t2;
		MatrIdentity_4x4(t2);
		t2[3][0]=_fontSize * 0.8f;
		MatrMul_4x4_4x4(t2, oldMV, MV);

		GL_CHECK_ERRORS 
    }  
	glBindVertexArray(0);
	GL_CHECK_ERRORS

	_font_shader.UnUse();

	GL_CHECK_ERRORS

	//if(test[0])
		glEnable(GL_DEPTH_TEST);
}