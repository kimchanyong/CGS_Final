#include "maze.h"
#include <fstream>
#include <iostream>
#include <string>
#include <sstream>

// 미로 데이터 전역 변수 정의
int MAZE[MAX_MAZE_SIZE][MAX_MAZE_SIZE] = { 0 };
int MAZE_ROWS = 0;
int MAZE_COLS = 0;

bool loadMaze(const char* filename) {
    std::ifstream file(filename);

    if (!file.is_open()) {
        std::cerr << "Error: Cannot open maze file: " << filename << std::endl;
        return false;
    }

    std::string line;
    int row = 0;
    int maxCols = 0;

    while (std::getline(file, line) && row < MAX_MAZE_SIZE) {
        std::istringstream iss(line);
        int col = 0;
        int value;

        while (iss >> value && col < MAX_MAZE_SIZE) {
            MAZE[row][col] = value;
            col++;
        }

        if (col > 0) {
            if (col > maxCols) maxCols = col;
            row++;
        }
    }

    MAZE_ROWS = row;
    MAZE_COLS = maxCols;
    file.close();

    std::cout << "Maze loaded successfully!" << std::endl;
    std::cout << "Size: " << MAZE_ROWS << " x " << MAZE_COLS << std::endl;

    return true;
}

void getStartPosition(float& x, float& z) {
    for (int i = 0; i < MAZE_ROWS; i++) {
        for (int j = 0; j < MAZE_COLS; j++) {
            if (MAZE[i][j] == TILE_START) {
                x = i * CELL_SIZE - (MAZE_ROWS * CELL_SIZE) / 2.0f;
                z = j * CELL_SIZE - (MAZE_COLS * CELL_SIZE) / 2.0f;
                std::cout << "Start position found at tile (" << i << ", " << j << ")" << std::endl;
                return;
            }
        }
    }

    std::cout << "No start position (-1) found, using first empty space..." << std::endl;
    for (int i = 0; i < MAZE_ROWS; i++) {
        for (int j = 0; j < MAZE_COLS; j++) {
            if (MAZE[i][j] == TILE_EMPTY) {
                x = i * CELL_SIZE - (MAZE_ROWS * CELL_SIZE) / 2.0f;
                z = j * CELL_SIZE - (MAZE_COLS * CELL_SIZE) / 2.0f;
                std::cout << "Using first empty space at tile (" << i << ", " << j << ")" << std::endl;
                return;
            }
        }
    }

    x = 0.0f;
    z = 0.0f;
}

void getGoalPosition(float& x, float& z) {
    for (int i = 0; i < MAZE_ROWS; i++) {
        for (int j = 0; j < MAZE_COLS; j++) {
            if (MAZE[i][j] == TILE_GOAL) {
                x = i * CELL_SIZE - (MAZE_ROWS * CELL_SIZE) / 2.0f;
                z = j * CELL_SIZE - (MAZE_COLS * CELL_SIZE) / 2.0f;
                std::cout << "Goal position found at tile (" << i << ", " << j << ")" << std::endl;
                return;
            }
        }
    }

    x = 0.0f;
    z = 0.0f;
    std::cout << "Warning: No goal position (-2) found!" << std::endl;
}

// 월드 좌표를 타일 좌표로 변환
void worldToTile(float worldX, float worldZ, int& tileI, int& tileJ) {
    tileI = (int)((worldX + (MAZE_ROWS * CELL_SIZE) / 2.0f) / CELL_SIZE);
    tileJ = (int)((worldZ + (MAZE_COLS * CELL_SIZE) / 2.0f) / CELL_SIZE);

    // 범위 체크
    if (tileI < 0) tileI = 0;
    if (tileI >= MAZE_ROWS) tileI = MAZE_ROWS - 1;
    if (tileJ < 0) tileJ = 0;
    if (tileJ >= MAZE_COLS) tileJ = MAZE_COLS - 1;
}

// 타일 좌표를 월드 좌표로 변환 (타일 중앙)
void tileToWorld(int tileI, int tileJ, float& worldX, float& worldZ) {
    worldX = tileI * CELL_SIZE - (MAZE_ROWS * CELL_SIZE) / 2.0f;
    worldZ = tileJ * CELL_SIZE - (MAZE_COLS * CELL_SIZE) / 2.0f;
}