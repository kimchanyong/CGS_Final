
#ifndef OBJECT_H
#define OBJECT_H

#include <vector>
#include <GL/glut.h>

struct Vec3 {
    float x, y, z;
};

class Mesh {
public:
    std::vector<Vec3> vertices;
    std::vector<Vec3> normals;
    std::vector<unsigned int> indices;

    void Draw();
    bool LoadSOR(const char* filename);
};

extern Mesh goblet1Mesh;
extern Mesh goblet2Mesh;
extern Mesh goblet3Mesh;

#endif