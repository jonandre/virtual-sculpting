#pragma once
#include <map>

#include <GL/glew.h>
#include <string>

using namespace std;

class Shader
{
public:
	Shader();
	~Shader();
	bool loadVertexShader( const char* name );
	bool loadFragmentShader( const char* name );
	bool loadGeometryShader( const char* name );

	/**
	 *	NEW
	 */
	void LoadFromFile(GLenum, const string);
	void LoadFromString(GLenum, const string);
	void Use();
	void UnUse();
	void AddAttribute(const string );
	GLuint operator [](const string );
	void AddUniform(const string );
	GLuint operator()(const string );


	void link();

	void bind();
	void unbind();
	unsigned int id();
private:
	//GLuint _shaders[3];//0->vertexshader, 1->fragmentshader, 2->geometryshader
	
	map<string,GLuint> _attributeList;
	map<string,GLuint> _uniformLocationList;

	unsigned int shader_id;
    unsigned int shader_vertexp;
    unsigned int shader_fragmentp;
	unsigned int shader_geometryp;
};