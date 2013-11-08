#pragma once
struct Point;
struct Color;
struct Normal;
class VBO;
class VAO;

class Mesh
{
public:
	Mesh();
	~Mesh();
	void CreateMesh( Point* verts, unsigned int vert_len, Color* clr, unsigned int clr_len, Normal* normals, unsigned int norm_len  );
private:
	VAO* _vao;
	VBO* _vbo;
};

