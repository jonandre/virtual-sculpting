#ifndef SPHERE_H
#define SPHERE_H

#include "main.h"
#include "GraphicsLib.h"

class Sphere
{
protected:
    std::vector<GLfloat> vertices;
    std::vector<GLfloat> normals;
    std::vector<GLfloat> texcoords;
    std::vector<GLushort> indices;

public:
	Sphere();

    Sphere(float radius, unsigned int rings, unsigned int sectors);

    void draw(GLfloat x, GLfloat y, GLfloat z);
};

#endif // SPHERE_H