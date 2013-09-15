#include "<stdio.h>"
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include "Shader.h"
#include "main.h"
#include "GraphicsLib.h"


static char* read_file(const char* fileName);


/**
 * Constructor
 */
Shader::Shader()
{
	this->shaderProgram  = ~0U;
	this->shaderVertex   = ~0U;
	this->shaderFragment = ~0U;
	this->shaderGeometry = ~0U;
}


/**
 * Destructor
 */
Shader::~Shader()
{
	#define destroy_shader(SHADER)                                                  \
		if (this->shader##SHADER != ~0U)                                        \
		{                                                                       \
			glDetachShader(this->shaderProgram, this->shader##SHADER);      \
			glDeleteShader(this->shader##SHADER);                           \
		}
	
	destroy_shader(Fragment)
	destroy_shader(Vertex)
	destroy_shader(Geometry)
	
	if (shaderProgram != ~0U)
		glDeleteProgram(shaderProgram);
	
	#undef destroy_shader
}


/**
 * Load shader
 * 
 * @param   name    The file name of the shader to load
 * @param   shader  Pointer to shader variable
 * @param   type    Shader type
 * @return          Whether a shader was loaded
 */
static bool load_shader(const char* name, unsigned int* shader, int type)
{
	GLint Result = GL_FALSE;
	int InfoLogLength;
	
	if (*shader == ~0U)
	{
		*shader = glCreateShader(type);
		const char* text = read_file(name);
		
		if (*text)
		{
			glShaderSource(*shader, 1, &text, 0);
			glCompileShader(*shader);
			free(text);
			
			glGetShaderiv(*shader, GL_COMPILE_STATUS, &Result);
			glGetShaderiv(*shader, GL_INFO_LOG_LENGTH, &InfoLogLength);
			
			if (InfoLogLength > 0)
			{
				std::vector<char> error_message(InfoLogLength + 1);
				glGetShaderInfoLog(*shader, InfoLogLength, NULL, &(error_message[0]));
				std::cerr << error_message.data() << std::endl;
			}
			
			return true;
		}
	}
	
	*shader = ~0U;
	return false;
}


/**
 * Load vertex shader
 * 
 * @param  name  The file name of the shader to load
 */
bool Shader::LoadVertexShader(const char* name)
{
	return load_shader(name, &(this->shaderVertex), GL_VERTEX_SHADER);
}

/**
 * Load fragment shader
 * 
 * @param  name  The file name of the shader to load
 */
bool Shader::LoadFragmentShader(const char* name)
{
	return load_shader(name, &(this->shaderFragment), GL_FRAGMENT_SHADER);
}

/**
 * Load geometry shader
 * 
 * @param  name  The file name of the shader to load
 */
bool Shader::LoadGeometryShader(const char* name)
{
	return load_shader(name, &(this->shaderGeometry), GL_GEOMETRY_SHADER);
}


void Shader::Link()
{
	this->shaderProgram = glCreateProgram();
	
	if (this->shaderVertex   != ~0U)  glAttachShader(this->shaderProgram, this->shaderVertex);
	if (this->shaderGeometry != ~0U)  glAttachShader(this->shaderProgram, this->shaderGeometry);
	if (this->shaderFragment != ~0U)  glAttachShader(this->shaderProgram, this->shaderFragment);
	
	glLinkProgram(this->shaderProgram);
}

void Shader::Bind()
{
	glUseProgram(this->shaderProgram);
}

void Shader::Unbind()
{
	glUseProgram(0);
}


/**
 * Read a text file
 * 
 * @param   filename  The file name of the file to read
 * @return            The content of the file
 */
static char* read_file(const char* filename)
{
	FILE* file;
	struct stat attr;
	char* content;
	long ptr = 0;
	
	/* Get file size and optimal I/O settings */
	if (stat(filename, &attr))
	{
		perror("stat");
		abort();
	}
	
	/* Open file */
	if ((file = fopen(filename, "r")) == NULL)
	{
		perror("fopen");
		abort();
	}
	
	/* Read file */
	content = (char*)malloc(attr.st_size);
	for (;;)
	{
		size_t read = fread(content + ptr, 1, attr->st_blksize, file);
		if (read != attr->st_blksize)
		{
			if (feof(file))
				break;
			else
			{
				perror("fread");
				abort();
			}
		}
		ptr += read;
	}
	
	/* Null terminate string and return */
	*(content + ptr) = 0;
	return content;
}

