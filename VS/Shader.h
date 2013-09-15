#pragma once


class Shader
{
public:
	/**
	 * Constructor
	 */
	Shader();
	
	/**
	 * Destructor
	 */
	~Shader();
	
	
	/**
	 * Load vertex shader
	 * 
	 * @param  name  The file name of the shader to load
	 */
	bool LoadVertexShader(const char* name);
	
	/**
	 * Load fragment shader
	 * 
	 * @param  name  The file name of the shader to load
	 */
	bool LoadFragmentShader(const char* name);
	
	/**
	 * Load geometry shader
	 * 
	 * @param  name  The file name of the shader to load
	 */
	bool LoadGeometryShader(const char* name);
	
	
	void Link();
	
	void Bind();
	void Unbind();
	
	unsigned int shaderProgram;
	
	
	
private:
	/**
	 * Vertex shader
	 */
	unsigned int shaderVertex;
	
	/**
	 * Fragment shader
	 */
	unsigned int shaderFragment;
	
	/**
	 * Geometry shader
	 */
	unsigned int shaderGeometry;
};

