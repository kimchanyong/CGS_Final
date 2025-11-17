#include "maze.h"
#include <fstream>
#include <iostream>
#include <string>

// 미로 데이터 전역 변수 정의
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

        // 외곽 벽
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

        MAZE[2][2] = TILE_ITEM;
        MAZE[4][7] = TILE_ITEM;
        MAZE[MAZE_ROWS - 2][MAZE_COLS - 2] = TILE_GOAL;

        return false;
    }

    std::string line;
    int row = 0;
    int maxCols = 0;

    while (std::getline(file, line) && row < MAX_MAZE_SIZE) {
        int col = 0;

        for (size_t i = 0; i < line.length() && col < MAX_MAZE_SIZE; i++) {
            char c = line[i];

            if (c >= '0' && c <= '9') {
                MAZE[row][col] = c - '0';
                col++;
            }
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

// 전체 아이템 개수 계산
int countTotalItems() {
    int count = 0;
    for (int i = 0; i < MAZE_ROWS; i++) {
        for (int j = 0; j < MAZE_COLS; j++) {
            if (MAZE[i][j] == TILE_ITEM) {
                count++;
            }
        }
    }
    std::cout << "Total items in maze: " << count << std::endl;
    return count;
}