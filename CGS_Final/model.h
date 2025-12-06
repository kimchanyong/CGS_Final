// model.h
#pragma once
#include <vector>

struct Vec2 {
    float x, y;
};

struct Vec3 {
    float x, y, z;
};

struct ObjVertex {
    Vec3 pos;
    Vec3 normal;
    Vec2 uv;
};

struct ObjMesh {
    std::vector<ObjVertex> vertices; // 삼각형 정점들 (3개마다 한 삼각형)
};

bool loadOBJ(const char* filename, ObjMesh& mesh);
void drawOBJ(const ObjMesh& mesh);
