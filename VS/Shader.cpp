#include "Shader.h"
#include "main.h"
#include "GraphicsLib.h"


Shader::Shader(): shader_fragmentp(-1), shader_vertexp(-1), shader_id(-1), shader_geometryp(-1)
{
	_attributeList.clear();
	_uniformLocationList.clear();

}

bool Shader::loadVertexShader( const char* name )
{
	GLint Result = GL_FALSE;
	int InfoLogLength;

	if ( shader_vertexp == -1 )
	{
		shader_vertexp = glCreateShader(GL_VERTEX_SHADER);
		string vsText = textFileRead(name);
		
		if ( vsText.length() )
		{
			const char *PointText = vsText.c_str();
			glShaderSource(shader_vertexp, 1, &PointText, 0);
			glCompileShader(shader_vertexp);

			glGetShaderiv(shader_vertexp, GL_COMPILE_STATUS, &Result);
			glGetShaderiv(shader_vertexp, GL_INFO_LOG_LENGTH, &InfoLogLength);
			if ( InfoLogLength > 0 ){
				std::vector<char> FragmentShaderErrorMessage(InfoLogLength+1);
				glGetShaderInfoLog(shader_vertexp, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);
				cout << FragmentShaderErrorMessage.data() << endl;
			}

			return true;
		}
	}
	shader_vertexp = -1;
	return false;
}

bool Shader::loadFragmentShader( const char* name )
{
	GLint Result = GL_FALSE;
	int InfoLogLength;

	if ( shader_fragmentp == -1 )
	{
		shader_fragmentp = glCreateShader(GL_FRAGMENT_SHADER);
		string fsText = textFileRead(name);
		

		if ( fsText.length() )
		{
			const char *fragmentText = fsText.c_str();
			glShaderSource(shader_fragmentp, 1, &fragmentText, 0);
			glCompileShader(shader_fragmentp);

			glGetShaderiv(shader_fragmentp, GL_COMPILE_STATUS, &Result);
			glGetShaderiv(shader_fragmentp, GL_INFO_LOG_LENGTH, &InfoLogLength);
			if ( InfoLogLength > 0 ){
				std::vector<char> FragmentShaderErrorMessage(InfoLogLength+1);
				glGetShaderInfoLog(shader_fragmentp, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);
				cout << FragmentShaderErrorMessage.data() << endl;
				//printf("%s\n", &FragmentShaderErrorMessage[0]);
			}

			return true;
		}
	}
	shader_fragmentp = -1;
	return false;
}

bool Shader::loadGeometryShader( const char* name )
{
	GLint Result = GL_FALSE;
	int InfoLogLength;

	if ( shader_geometryp == -1 )
	{
		shader_geometryp = glCreateShader(GL_GEOMETRY_SHADER);
		string fsText = textFileRead(name);
		

		if ( fsText.length() )
		{
			const char *fragmentText = fsText.c_str();
			glShaderSource(shader_geometryp, 1, &fragmentText, 0);
			glCompileShader(shader_geometryp);

			glGetShaderiv(shader_geometryp, GL_COMPILE_STATUS, &Result);
			glGetShaderiv(shader_geometryp, GL_INFO_LOG_LENGTH, &InfoLogLength);
			if ( InfoLogLength > 0 ){
				std::vector<char> FragmentShaderErrorMessage(InfoLogLength+1);
				glGetShaderInfoLog(shader_geometryp, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);
				cout << FragmentShaderErrorMessage.data() << endl;
			}
			return true;
		}
	}
	shader_geometryp = -1;
	return false;
}

void Shader::link()
{
	shader_id = glCreateProgram();

	if ( shader_vertexp != -1 )
		glAttachShader(shader_id, shader_vertexp);

	if ( shader_geometryp != -1 )
		glAttachShader(shader_id, shader_geometryp);
	
	if ( shader_fragmentp != -1 )
		glAttachShader(shader_id, shader_fragmentp);
	
	glLinkProgram(shader_id);
}

Shader::~Shader()
{
	if ( shader_fragmentp != -1 )
	{
		glDetachShader(shader_id, shader_fragmentp);
		glDeleteShader(shader_fragmentp);
	}

	if ( shader_vertexp != -1 )
	{
		glDetachShader(shader_id, shader_vertexp);
		glDeleteShader(shader_vertexp);
	}

	if ( shader_geometryp != -1 )
	{
		glDetachShader(shader_id, shader_geometryp);
		glDeleteShader(shader_geometryp);
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