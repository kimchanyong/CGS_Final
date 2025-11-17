#ifndef MAZE_H
#define MAZE_H

#include "util.h"

// 미로 데이터 (extern으로 선언)
extern int MAZE[MAX_MAZE_SIZE][MAX_MAZE_SIZE];
extern int MAZE_ROWS;
extern int MAZE_COLS;

// 미로 타일 타입
enum TileType {
    TILE_EMPTY = 0,
    TILE_WALL = 1,
    TILE_ITEM = 2,
    TILE_GOAL = 3
};

// 미로 로딩 함수
bool loadMaze(const char* filename);
int countTotalItems();

// 미로 렌더링 함수
void drawMaze();
void drawWall();
void drawFloorTile();
void drawItem();
void drawGoal();

#endif