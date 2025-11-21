#ifndef MAZE_H
#define MAZE_H

#include "util.h"

// 미로 데이터
extern int MAZE[MAX_MAZE_SIZE][MAX_MAZE_SIZE];
extern int MAZE_ROWS;
extern int MAZE_COLS;

// 미로 타일 타입 (새로운 정의)
enum TileType {
    TILE_GOAL = -2,        // 탈출 지점
    TILE_START = -1,       // 출발 지점
    TILE_EMPTY = 0,        // 빈 공간
    TILE_WALL = 1,         // 벽
    TILE_RED_KEY = 2,      // 빨간 열쇠
    TILE_BLUE_KEY = 3,     // 파란 열쇠
    TILE_YELLOW_KEY = 4    // 노란 열쇠
};

// 미로 로딩 함수
bool loadMaze(const char* filename);
void getStartPosition(float& x, float& z);
void getGoalPosition(float& x, float& z);

// 미로 렌더링 함수
void drawMaze();
void drawWall();
void drawFloorTile();
void drawRedKey();
void drawBlueKey();
void drawYellowKey();
void drawGoal(bool activated);  // activated 파라미터 추가

#endif