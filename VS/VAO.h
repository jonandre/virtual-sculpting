class VBO;

/**
 * Vertex array object
 */
class VAO
{
public:
	VAO();
	~VAO();
	unsigned int id();
	unsigned int size();
	void bind(const VBO& vbo);
	
private:
	unsigned int vaoID[1];
	unsigned int sz;
	unsigned int counter;
};

