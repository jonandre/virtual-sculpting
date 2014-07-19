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

#pragma once
#include <GL/glew.h>
#include <map>
#include <string>

using namespace std;

class GLSLShader
{
public:
	GLSLShader(void);
	~GLSLShader(void);	
	void LoadFromString(GLenum whichShader, const string source);
	void LoadFromFile(GLenum whichShader, const string filename);
	void CreateAndLinkProgram();
	void Use();
	void UnUse();
	void AddAttribute(const string attribute);
	void AddUniform(const string uniform);

	GLuint GetProgram()const;

	//An indexer that returns the location of the attribute/uniform
	GLuint operator[](const string attribute);
	GLuint operator()(const string uniform);
private:
	enum ShaderType {VERTEX_SHADER, FRAGMENT_SHADER, GEOMETRY_SHADER};
	GLuint	_program;
	int _totalShaders;
	GLuint _shaders[3];//0->vertexshader, 1->fragmentshader, 2->geometryshader
	map<string,GLuint> _attributeList;
	map<string,GLuint> _uniformLocationList;
};	



