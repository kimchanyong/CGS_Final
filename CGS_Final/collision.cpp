#include "collision.h"
#include "maze.h"
#include <iostream>

AABB wallBoxes[MAX_MAZE_SIZE][MAX_MAZE_SIZE];

void buildCollisionMap() {
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

KeyCollection checkKeyCollision(float x, float z, float radius) {
    KeyCollection collected;

    for (int i = 0; i < MAZE_ROWS; i++) {
        for (int j = 0; j < MAZE_COLS; j++) {
            int tile = MAZE[i][j];

            if (tile == TILE_RED_KEY || tile == TILE_BLUE_KEY || tile == TILE_YELLOW_KEY) {
                float keyX = i * CELL_SIZE - (MAZE_ROWS * CELL_SIZE) / 2.0f;
                float keyZ = j * CELL_SIZE - (MAZE_COLS * CELL_SIZE) / 2.0f;

                float dx = x - keyX;
                float dz = z - keyZ;
                float distSquared = dx * dx + dz * dz;
                float collectDist = radius + 0.5f;

                if (distSquared < collectDist * collectDist) {
                    if (tile == TILE_RED_KEY) {
                        collected.redKey = true;
                        MAZE[i][j] = TILE_EMPTY;
                        std::cout << "RED KEY collected!" << std::endl;
                    }
                    else if (tile == TILE_BLUE_KEY) {
                        collected.blueKey = true;
                        MAZE[i][j] = TILE_EMPTY;
                        std::cout << "BLUE KEY collected!" << std::endl;
                    }
                    else if (tile == TILE_YELLOW_KEY) {
                        collected.yellowKey = true;
                        MAZE[i][j] = TILE_EMPTY;
                        std::cout << "YELLOW KEY collected!" << std::endl;
                    }
                }
            }
        }
    }

    return collected;
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