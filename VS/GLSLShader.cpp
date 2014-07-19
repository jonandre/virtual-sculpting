/*
 * ---------------- www.spacesimulator.net --------------
 *   ---- Space simulators and 3d engine tutorials ----
 *
 * Original Author: Damiano Vitulli
 * Porting to OpenGL3.3: Movania Muhammad Mobeen
 * Shaders Functions: Movania Muhammad Mobeen
 *
 * This program is released under the BSD licence
 * By using this program you agree to licence terms on spacesimulator.net copyright page
 *
 */

#include "GLSLShader.h"
#include <iostream>


GLSLShader::GLSLShader(void)
{
	_totalShaders=0;
	_shaders[VERTEX_SHADER]=0;
	_shaders[FRAGMENT_SHADER]=0;
	_shaders[GEOMETRY_SHADER]=0;
	_attributeList.clear();
	_uniformLocationList.clear();
}

GLSLShader::~GLSLShader(void)
{
	_attributeList.clear();	
	_uniformLocationList.clear();
	glDeleteProgram(_program);
}


/**
 * Added
 */
void GLSLShader::LoadFromString(GLenum type, const string source) {	
	GLuint shader = glCreateShader (type);

	const char * ptmp = source.c_str();
	glShaderSource (shader, 1, &ptmp, NULL);
	
	//check whether the shader loads fine
	GLint status;
	glCompileShader (shader);
	glGetShaderiv (shader, GL_COMPILE_STATUS, &status);
	if (status == GL_FALSE) {
		GLint infoLogLength;		
		glGetShaderiv (shader, GL_INFO_LOG_LENGTH, &infoLogLength);
		GLchar *infoLog= new GLchar[infoLogLength];
		glGetShaderInfoLog (shader, infoLogLength, NULL, infoLog);
		cerr<<"Compile log: "<<infoLog<<endl;
		delete [] infoLog;
	}
	_shaders[_totalShaders++]=shader;
}

/**
 * Skipped
 */
void GLSLShader::CreateAndLinkProgram() {
	_program = glCreateProgram ();
	if (_shaders[VERTEX_SHADER] != 0) {
		glAttachShader (_program, _shaders[VERTEX_SHADER]);
	}
	if (_shaders[FRAGMENT_SHADER] != 0) {
		glAttachShader (_program, _shaders[FRAGMENT_SHADER]);
	}
	if (_shaders[GEOMETRY_SHADER] != 0) {
		glAttachShader (_program, _shaders[GEOMETRY_SHADER]);
	}
	
	//link and check whether the program links fine
	GLint status;
	glLinkProgram (_program);
	glGetProgramiv (_program, GL_LINK_STATUS, &status);
	if (status == GL_FALSE) {
		GLint infoLogLength;
		
		glGetProgramiv (_program, GL_INFO_LOG_LENGTH, &infoLogLength);
		GLchar *infoLog= new GLchar[infoLogLength];
		glGetProgramInfoLog (_program, infoLogLength, NULL, infoLog);
		cerr<<"Link log: "<<infoLog<<endl;
		delete [] infoLog;
	}

	glDeleteShader(_shaders[VERTEX_SHADER]);
	glDeleteShader(_shaders[FRAGMENT_SHADER]);
	glDeleteShader(_shaders[GEOMETRY_SHADER]);
}







/**
 * Added
 */
void GLSLShader::Use() {
	glUseProgram(_program);
}

/**
 * Added
 */
void GLSLShader::UnUse() {
	glUseProgram(0);
}

/**
 * Added
 */
void GLSLShader::AddAttribute(const string attribute) {
	_attributeList[attribute]= glGetAttribLocation(_program, attribute.c_str());	
}

/**
 * Added
 */
//An indexer that returns the location of the attribute
GLuint GLSLShader::operator [](const string attribute) {
	return _attributeList[attribute];
}

/**
 * Added
 */
void GLSLShader::AddUniform(const string uniform) {
	_uniformLocationList[uniform] = glGetUniformLocation(_program, uniform.c_str());
}

/**
 * Added
 */
GLuint GLSLShader::operator()(const string uniform){
	return _uniformLocationList[uniform];
}

/**
 * Added
 */
GLuint GLSLShader::GetProgram() const {
	return _program;
}


/**
 * Added
 */
#include <fstream>
void GLSLShader::LoadFromFile(GLenum whichShader, const string filename){
	ifstream fp;
	fp.open(filename.c_str(), ios_base::in);
	if(fp) {		 
		/*string line, buffer;
		while(getline(fp, line)) {
			buffer.append(line);
			buffer.append("\r\n");
		}		*/
		string buffer(std::istreambuf_iterator<char>(fp), (std::istreambuf_iterator<char>()));
		//copy to source
		LoadFromString(whichShader, buffer);		
	} else {
		cerr<<"Error loading shader: "<<filename<<endl;
	}
}
