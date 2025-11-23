#include "object.h"
#include <cmath>
#include <fstream>
#include <iostream>

void Mesh::Draw() {
    if (vertices.empty() || indices.empty())
        return;

    glBegin(GL_TRIANGLES);
    for (size_t i = 0; i < indices.size(); i += 3) {

        for (int k = 0; k < 3; k++) {
            unsigned int id = indices[i + k];

            Vec3 n = normals[id];
            Vec3 v = vertices[id];

            glNormal3f(n.x, n.y, n.z);
            glVertex3f(v.x, v.y, v.z);
        }
    }
    glEnd();
}

bool Mesh::LoadSOR(const char* filename) {
    std::ifstream in(filename);
    if (!in.is_open()) {
        std::cout << "SOR file open failed: " << filename << std::endl;
        return false;
    }

    int P;
    in >> P;

    std::vector<float> r(P), y(P);
    for (int i = 0; i < P; i++) {
        in >> r[i] >> y[i];
    }
    in.close();

    vertices.clear();
    normals.clear();
    indices.clear();

    int slices = 48;
    float TWO_PI = 6.28318530718f;

    for (int s = 0; s < slices; s++) {
        float theta = TWO_PI * s / slices;
        float c = cos(theta);
        float si = sin(theta);

        for (int p = 0; p < P; p++) {
            vertices.push_back({
                r[p] * c,
                y[p],
                r[p] * si
                });
        }
    }

    auto idx = [P](int s, int p) {
        return (unsigned int)(s * P + p);
        };

    for (int s = 0; s < slices; s++) {
        int sNext = (s + 1) % slices;
        for (int p = 0; p < P - 1; p++) {
            unsigned int v0 = idx(s, p);
            unsigned int v1 = idx(sNext, p);
            unsigned int v2 = idx(sNext, p + 1);
            unsigned int v3 = idx(s, p + 1);

            indices.push_back(v0);
            indices.push_back(v1);
            indices.push_back(v2);
            indices.push_back(v0);
            indices.push_back(v2);
            indices.push_back(v3);
        }
    }

    normals.assign(vertices.size(), { 0,0,0 });

    for (size_t i = 0; i < indices.size(); i += 3) {
        unsigned int ia = indices[i];
        unsigned int ib = indices[i + 1];
        unsigned int ic = indices[i + 2];

        Vec3 a = vertices[ia];
        Vec3 b = vertices[ib];
        Vec3 c = vertices[ic];

        Vec3 n = {
            (b.y - a.y) * (c.z - a.z) - (b.z - a.z) * (c.y - a.y),
            (b.z - a.z) * (c.x - a.x) - (b.x - a.x) * (c.z - a.z),
            (b.x - a.x) * (c.y - a.y) - (b.y - a.y) * (c.x - a.x)
        };

        normals[ia] = { normals[ia].x + n.x, normals[ia].y + n.y, normals[ia].z + n.z };
        normals[ib] = { normals[ib].x + n.x, normals[ib].y + n.y, normals[ib].z + n.z };
        normals[ic] = { normals[ic].x + n.x, normals[ic].y + n.y, normals[ic].z + n.z };
    }

    for (auto& n : normals) {
        float L = sqrt(n.x * n.x + n.y * n.y + n.z * n.z);
        if (L > 1e-6f) {
            n.x /= L; n.y /= L; n.z /= L;
        }
    }

    return true;
}
