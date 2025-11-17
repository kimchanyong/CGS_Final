#include "collision.h"
#include "maze.h"
#include <iostream>

AABB wallBoxes[MAX_MAZE_SIZE][MAX_MAZE_SIZE];

void buildCollisionMap() {
    // 벽 충돌 박스 크기 = CELL_SIZE
    float wallSize = CELL_SIZE;

    for (int i = 0; i < MAZE_ROWS; i++) {
        for (int j = 0; j < MAZE_COLS; j++) {
            if (MAZE[i][j] == TILE_WALL) {
                float cx = i * CELL_SIZE - (MAZE_ROWS * CELL_SIZE) / 2.0f;
                float cz = j * CELL_SIZE - (MAZE_COLS * CELL_SIZE) / 2.0f;

                wallBoxes[i][j] = AABB(
                    cx - wallSize / 2.0f,
                    cx + wallSize / 2.0f,
                    cz - wallSize / 2.0f,
                    cz + wallSize / 2.0f,
                    true
                );
            }
            else {
                wallBoxes[i][j].active = false;
            }
        }
    }

    std::cout << "Collision map built successfully" << std::endl;
}

bool checkPointInAABB(float x, float z, const AABB& box) {
    if (!box.active) return false;
    return (x >= box.minX && x <= box.maxX && z >= box.minZ && z <= box.maxZ);
}

bool checkCollision(float x, float z, float radius) {
    for (int i = 0; i < MAZE_ROWS; i++) {
        for (int j = 0; j < MAZE_COLS; j++) {
            if (!wallBoxes[i][j].active) continue;

            const AABB& box = wallBoxes[i][j];

            float closestX = x;
            float closestZ = z;

            if (x < box.minX) closestX = box.minX;
            else if (x > box.maxX) closestX = box.maxX;

            if (z < box.minZ) closestZ = box.minZ;
            else if (z > box.maxZ) closestZ = box.maxZ;

            float dx = x - closestX;
            float dz = z - closestZ;
            float distSquared = dx * dx + dz * dz;

            if (distSquared < radius * radius) {
                return true;
            }
        }
    }
    return false;
}

int checkItemCollision(float x, float z, float radius) {
    int itemsCollected = 0;

    for (int i = 0; i < MAZE_ROWS; i++) {
        for (int j = 0; j < MAZE_COLS; j++) {
            if (MAZE[i][j] == TILE_ITEM) {
                float itemX = i * CELL_SIZE - (MAZE_ROWS * CELL_SIZE) / 2.0f;
                float itemZ = j * CELL_SIZE - (MAZE_COLS * CELL_SIZE) / 2.0f;

                float dx = x - itemX;
                float dz = z - itemZ;
                float distSquared = dx * dx + dz * dz;
                float collectDist = radius + 0.5f;

                if (distSquared < collectDist * collectDist) {
                    MAZE[i][j] = TILE_EMPTY;
                    itemsCollected++;
                    std::cout << "Item collected!" << std::endl;
                }
            }
        }
    }
    return itemsCollected;
}

bool checkGoalCollision(float x, float z, float radius) {
    for (int i = 0; i < MAZE_ROWS; i++) {
        for (int j = 0; j < MAZE_COLS; j++) {
            if (MAZE[i][j] == TILE_GOAL) {
                float goalX = i * CELL_SIZE - (MAZE_ROWS * CELL_SIZE) / 2.0f;
                float goalZ = j * CELL_SIZE - (MAZE_COLS * CELL_SIZE) / 2.0f;

                float dx = x - goalX;
                float dz = z - goalZ;
                float distSquared = dx * dx + dz * dz;
                float goalDist = radius + 0.6f;

                if (distSquared < goalDist * goalDist) {
                    return true;
                }
            }
        }
    }
    return false;
}