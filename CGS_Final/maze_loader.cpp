#include "maze.h"
#include <fstream>
#include <iostream>
#include <string>
#include <sstream>

// 미로 데이터 전역 변수
int MAZE[MAX_MAZE_SIZE][MAX_MAZE_SIZE] = { 0 };
int MAZE_ROWS = 0;
int MAZE_COLS = 0;

bool loadMaze(const char* filename) {
    std::ifstream file(filename);

    if (!file.is_open()) {
        std::cerr << "Error: Cannot open maze file: " << filename << std::endl;
        std::cerr << "Creating default maze..." << std::endl;

        // 기본 미로 생성
        MAZE_ROWS = 10;
        MAZE_COLS = 10;

        for (int i = 0; i < MAZE_ROWS; i++) {
            for (int j = 0; j < MAZE_COLS; j++) {
                if (i == 0 || i == MAZE_ROWS - 1 || j == 0 || j == MAZE_COLS - 1) {
                    MAZE[i][j] = TILE_WALL;
                }
                else {
                    MAZE[i][j] = TILE_EMPTY;
                }
            }
        }

        MAZE[1][1] = TILE_START;
        MAZE[2][2] = TILE_RED_KEY;
        MAZE[4][4] = TILE_BLUE_KEY;
        MAZE[6][6] = TILE_YELLOW_KEY;
        MAZE[MAZE_ROWS - 2][MAZE_COLS - 2] = TILE_GOAL;

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

// 각 종류별 열쇠 개수 계산
void countItems(int& redKeys, int& blueKeys, int& yellowKeys) {
    redKeys = 0;
    blueKeys = 0;
    yellowKeys = 0;

    for (int i = 0; i < MAZE_ROWS; i++) {
        for (int j = 0; j < MAZE_COLS; j++) {
            if (MAZE[i][j] == TILE_RED_KEY) redKeys++;
            else if (MAZE[i][j] == TILE_BLUE_KEY) blueKeys++;
            else if (MAZE[i][j] == TILE_YELLOW_KEY) yellowKeys++;
        }
    }

    std::cout << "Items in maze:" << std::endl;
    std::cout << "  Red Keys: " << redKeys << std::endl;
    std::cout << "  Blue Keys: " << blueKeys << std::endl;
    std::cout << "  Yellow Keys: " << yellowKeys << std::endl;
}

// 시작 위치 찾기
void getStartPosition(float& x, float& z) {
    for (int i = 0; i < MAZE_ROWS; i++) {
        for (int j = 0; j < MAZE_COLS; j++) {
            if (MAZE[i][j] == TILE_START) {
                x = i * CELL_SIZE - (MAZE_ROWS * CELL_SIZE) / 2.0f;
                z = j * CELL_SIZE - (MAZE_COLS * CELL_SIZE) / 2.0f;
                std::cout << "Start position found at tile (" << i << ", " << j << ")" << std::endl;
                std::cout << "World coordinates: (" << x << ", " << z << ")" << std::endl;
                return;
            }
        }
    }

    std::cout << "No start position (-1) found, searching for first empty space..." << std::endl;
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

    std::cout << "Warning: No empty space found! Using center (0, 0)" << std::endl;
    x = 0.0f;
    z = 0.0f;
}