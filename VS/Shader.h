
class Shader
{
public:
	Shader();
	~Shader();
	bool loadVertexShader( const char* name );
	bool loadFragmentShader( const char* name );
	bool loadGeometryShader( const char* name );

	void link();

	void bind();
	void unbind();
	unsigned int id();
private:
	unsigned int shader_id;
    unsigned int shader_vp;
    unsigned int shader_fp;
	unsigned int shader_gp;
};