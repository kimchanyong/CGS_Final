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

// 열쇠 수집 상태
struct KeyCollection {
    bool redKey;
    bool blueKey;
    bool yellowKey;

    KeyCollection() : redKey(false), blueKey(false), yellowKey(false) {}

    bool hasAllKeys() const {
        return redKey && blueKey && yellowKey;
    }

    int getCount() const {
        return (redKey ? 1 : 0) + (blueKey ? 1 : 0) + (yellowKey ? 1 : 0);
    }
};

// 충돌 맵
extern AABB wallBoxes[MAX_MAZE_SIZE][MAX_MAZE_SIZE];

// 충돌 시스템 함수
void buildCollisionMap();
bool checkCollision(float x, float z, float radius);

// 열쇠 수집
KeyCollection checkKeyCollision(float x, float z, float radius);

// 골 충돌 체크
bool checkGoalCollision(float x, float z, float radius);

#endif