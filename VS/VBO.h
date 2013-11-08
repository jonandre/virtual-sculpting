struct Point;
struct Color;
struct Normal;

class VBO
{
public:
	VBO( const Point* vrt, const Color* clr, unsigned int* indices, unsigned int len, unsigned int indices_len = 0, float* uv = 0, const Normal* nrm = 0 );
	~VBO();
	unsigned int size() const;
	void UpdateColorArray(const Color* clr, unsigned int len);
	void UpdateIndexArray(unsigned int* indices, unsigned int len);
	void UpdateVertexArray(const Point* vrt, unsigned int len);
	const unsigned int bind ( unsigned int start = 0 ) const;
private:
	VBO();
	unsigned int vboID[5];
	//unsigned int *vboID;
	unsigned int sz;
	unsigned int vbo_cnt;
};