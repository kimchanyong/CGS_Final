#ifndef COLLISION_H
#define COLLISION_H

#include "util.h"

// AABB 구조체
struct AABB {
    float minX, maxX;
    float minZ, maxZ;
    bool active;

    AABB() : minX(0), maxX(0), minZ(0), maxZ(0), active(false) {}

    AABB(float minX, float maxX, float minZ, float maxZ, bool active = true)
        : minX(minX), maxX(maxX), minZ(minZ), maxZ(maxZ), active(active) {
    }
};

// 충돌 맵
extern AABB wallBoxes[MAX_MAZE_SIZE][MAX_MAZE_SIZE];

// 충돌 시스템 함수
void buildCollisionMap();
bool checkCollision(float x, float z, float radius);
bool checkPointInAABB(float x, float z, const AABB& box);

// 아이템/골 충돌 체크
int checkItemCollision(float x, float z, float radius);
bool checkGoalCollision(float x, float z, float radius);

#endif