#include "Shader.h"
#include "main.h"
#include "GraphicsLib.h"


Shader::Shader(): shader_fp(-1), shader_vp(-1), shader_id(-1), shader_gp(-1)
{

}

bool Shader::loadVertexShader( const char* name )
{
	GLint Result = GL_FALSE;
	int InfoLogLength;

	if ( shader_vp == -1 )
	{
		shader_vp = glCreateShader(GL_VERTEX_SHADER);
		string vsText = textFileRead(name);
		
		if ( vsText.length() )
		{
			const char *PointText = vsText.c_str();
			glShaderSource(shader_vp, 1, &PointText, 0);
			glCompileShader(shader_vp);

			glGetShaderiv(shader_vp, GL_COMPILE_STATUS, &Result);
			glGetShaderiv(shader_vp, GL_INFO_LOG_LENGTH, &InfoLogLength);
			if ( InfoLogLength > 0 ){
				std::vector<char> FragmentShaderErrorMessage(InfoLogLength+1);
				glGetShaderInfoLog(shader_vp, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);
				cout << FragmentShaderErrorMessage.data() << endl;
			}

			return true;
		}
	}
	shader_vp = -1;
	return false;
}

bool Shader::loadFragmentShader( const char* name )
{
	GLint Result = GL_FALSE;
	int InfoLogLength;

	if ( shader_fp == -1 )
	{
		shader_fp = glCreateShader(GL_FRAGMENT_SHADER);
		string fsText = textFileRead(name);
		

		if ( fsText.length() )
		{
			const char *fragmentText = fsText.c_str();
			glShaderSource(shader_fp, 1, &fragmentText, 0);
			glCompileShader(shader_fp);

			glGetShaderiv(shader_fp, GL_COMPILE_STATUS, &Result);
			glGetShaderiv(shader_fp, GL_INFO_LOG_LENGTH, &InfoLogLength);
			if ( InfoLogLength > 0 ){
				std::vector<char> FragmentShaderErrorMessage(InfoLogLength+1);
				glGetShaderInfoLog(shader_fp, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);
				cout << FragmentShaderErrorMessage.data() << endl;
				//printf("%s\n", &FragmentShaderErrorMessage[0]);
			}

			return true;
		}
	}
	shader_fp = -1;
	return false;
}

bool Shader::loadGeometryShader( const char* name )
{
	GLint Result = GL_FALSE;
	int InfoLogLength;

	if ( shader_gp == -1 )
	{
		shader_gp = glCreateShader(GL_GEOMETRY_SHADER);
		string fsText = textFileRead(name);
		

		if ( fsText.length() )
		{
			const char *fragmentText = fsText.c_str();
			glShaderSource(shader_gp, 1, &fragmentText, 0);
			glCompileShader(shader_gp);

			glGetShaderiv(shader_gp, GL_COMPILE_STATUS, &Result);
			glGetShaderiv(shader_gp, GL_INFO_LOG_LENGTH, &InfoLogLength);
			if ( InfoLogLength > 0 ){
				std::vector<char> FragmentShaderErrorMessage(InfoLogLength+1);
				glGetShaderInfoLog(shader_gp, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);
				cout << FragmentShaderErrorMessage.data() << endl;
				//printf("%s\n", &FragmentShaderErrorMessage[0]);
			}

			return true;
		}
	}
	shader_gp = -1;
	return false;
}

void Shader::link()
{
	shader_id = glCreateProgram();

	if ( shader_vp != -1 )
	{
		glAttachShader(shader_id, shader_vp);
	}

	if ( shader_gp != -1 )
	{
		glAttachShader(shader_id, shader_gp);
	}
	
	if ( shader_fp != -1 )
	{
		glAttachShader(shader_id, shader_fp);
	}
	
	glLinkProgram(shader_id);
}

Shader::~Shader()
{
	if ( shader_fp != -1 )
	{
		glDetachShader(shader_id, shader_fp);
		glDeleteShader(shader_fp);
	}

	if ( shader_vp != -1 )
	{
		glDetachShader(shader_id, shader_vp);
		glDeleteShader(shader_vp);
	}

	if ( shader_gp != -1 )
	{
		glDetachShader(shader_id, shader_gp);
		glDeleteShader(shader_gp);
	}
    
	if ( shader_id != -1 )
		glDeleteProgram(shader_id);
}

void Shader::bind()
{
	glUseProgram(shader_id);
}

void Shader::unbind()
{
	glUseProgram(0);
}

unsigned int Shader::id() 
{
    return shader_id;
}