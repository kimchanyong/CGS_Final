// model.cpp
#include "model.h"
#include <fstream>
#include <sstream>
#include <string>
#include <iostream>
#include <GL/glut.h>
#include <cmath>

static void computeFaceNormal(
    const Vec3& a, const Vec3& b, const Vec3& c, Vec3& n)
{
    Vec3 u{ b.x - a.x, b.y - a.y, b.z - a.z };
    Vec3 v{ c.x - a.x, c.y - a.y, c.z - a.z };
    n.x = u.y * v.z - u.z * v.y;
    n.y = u.z * v.x - u.x * v.z;
    n.z = u.x * v.y - u.y * v.x;
    float len = std::sqrt(n.x * n.x + n.y * n.y + n.z * n.z);
    if (len > 0.00001f) {
        n.x /= len; n.y /= len; n.z /= len;
    }
}

// "vIndex/vtIndex/vnIndex" 또는 "vIndex//vnIndex" 같은 문자열 파싱
static void parseFaceToken(
    const std::string& token,
    int& vi, int& ti, int& ni)
{
    vi = ti = ni = -1;

    // 슬래시 위치 찾기
    size_t firstSlash = token.find('/');
    size_t secondSlash = token.find('/', firstSlash == std::string::npos ? firstSlash : firstSlash + 1);

    if (firstSlash == std::string::npos) {
        // "v"
        vi = std::stoi(token);
    }
    else if (secondSlash == std::string::npos) {
        // "v/vt"
        vi = std::stoi(token.substr(0, firstSlash));
        std::string vtStr = token.substr(firstSlash + 1);
        if (!vtStr.empty())
            ti = std::stoi(vtStr);
    }
    else {
        // "v//vn" or "v/vt/vn"
        vi = std::stoi(token.substr(0, firstSlash));
        if (secondSlash > firstSlash + 1) {
            std::string vtStr = token.substr(firstSlash + 1, secondSlash - firstSlash - 1);
            if (!vtStr.empty())
                ti = std::stoi(vtStr);
        }
        std::string vnStr = token.substr(secondSlash + 1);
        if (!vnStr.empty())
            ni = std::stoi(vnStr);
    }
}

bool loadOBJ(const char* filename, ObjMesh& mesh)
{
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Cannot open OBJ file: " << filename << std::endl;
        return false;
    }

    std::vector<Vec3> positions;
    std::vector<Vec3> normals;
    std::vector<Vec2> texcoords;

    mesh.vertices.clear();

    std::string line;
    while (std::getline(file, line)) {
        if (line.empty() || line[0] == '#')
            continue;

        std::istringstream iss(line);
        std::string tag;
        iss >> tag;

        if (tag == "v") {
            Vec3 p;
            iss >> p.x >> p.y >> p.z;
            positions.push_back(p);
        }
        else if (tag == "vt") {
            Vec2 t;
            iss >> t.x >> t.y;
            texcoords.push_back(t);
        }
        else if (tag == "vn") {
            Vec3 n;
            iss >> n.x >> n.y >> n.z;
            normals.push_back(n);
        }
        else if (tag == "f") {
            std::vector<std::string> tokens;
            std::string tok;
            while (iss >> tok) tokens.push_back(tok);

            if (tokens.size() < 3) continue;

            // 정점 인덱스들 파싱 (삼각형/사각형 지원)
            std::vector<int> vIdx(tokens.size());
            std::vector<int> tIdx(tokens.size());
            std::vector<int> nIdx(tokens.size());

            for (size_t k = 0; k < tokens.size(); ++k) {
                parseFaceToken(tokens[k], vIdx[k], tIdx[k], nIdx[k]);

                // OBJ 인덱스는 1부터 시작 → 0부터로 변경
                if (vIdx[k] > 0) vIdx[k]--;
                if (tIdx[k] > 0) tIdx[k]--;
                if (nIdx[k] > 0) nIdx[k]--;
            }

            auto emitTriangle = [&](int a, int b, int c) {
                ObjVertex tri[3];

                int idx[3] = { a, b, c };
                for (int m = 0; m < 3; ++m) {
                    int vi = vIdx[idx[m]];
                    tri[m].pos = positions[vi];

                    if (!texcoords.empty() && tIdx[idx[m]] >= 0) {
                        tri[m].uv = texcoords[tIdx[idx[m]]];
                    }
                    else {
                        tri[m].uv = { 0.0f, 0.0f };
                    }

                    if (!normals.empty() && nIdx[idx[m]] >= 0) {
                        tri[m].normal = normals[nIdx[idx[m]]];
                    }
                    else {
                        tri[m].normal = { 0,0,0 };
                    }
                }

                // 노멀 없으면 face 노멀 계산해서 채워 넣기
                if (normals.empty() || nIdx[a] < 0 || nIdx[b] < 0 || nIdx[c] < 0) {
                    Vec3 n;
                    computeFaceNormal(tri[0].pos, tri[1].pos, tri[2].pos, n);
                    for (int m = 0; m < 3; ++m)
                        tri[m].normal = n;
                }

                for (int m = 0; m < 3; ++m)
                    mesh.vertices.push_back(tri[m]);
                };

            // 삼각형
            if (tokens.size() == 3) {
                emitTriangle(0, 1, 2);
            }
            // 사각형 → 삼각형 2개
            else if (tokens.size() == 4) {
                emitTriangle(0, 1, 2);
                emitTriangle(0, 2, 3);
            }
            else {
                // 5각형 이상은 과제용이면 그냥 무시하거나 삼각분할 더 구현해도 됨
                std::cerr << "Face with " << tokens.size()
                    << " vertices ignored in " << filename << std::endl;
            }
        }
    }

    std::cout << "OBJ loaded: " << filename
        << " (vertices=" << mesh.vertices.size() << ")\n";
    return true;
}

void drawOBJ(const ObjMesh& mesh)
{
    glBegin(GL_TRIANGLES);
    for (const auto& v : mesh.vertices) {
        glNormal3f(v.normal.x, v.normal.y, v.normal.z);
        glTexCoord2f(v.uv.x, v.uv.y);
        glVertex3f(v.pos.x, v.pos.y, v.pos.z);
    }
    glEnd();
}
