#ifndef MAZE_H
#define MAZE_H

#include "util.h"
#include <GL/glut.h> 

struct Material {
    float ambient[4];
    float diffuse[4];
    float specular[4];
    float shininess;

};

// 미로 데이터
extern int MAZE[MAX_MAZE_SIZE][MAX_MAZE_SIZE];
extern int MAZE_ROWS;
extern int MAZE_COLS;
extern Material g_wallMaterials[3];
extern int g_currentWallMaterial;

// 미로 타일 타입
enum TileType {
    TILE_GOAL = -2,   // 탈출 지점
    TILE_START = -1,   // 출발 지점
    TILE_EMPTY = 0,    // 빈 공간
    TILE_WALL = 1,    // 벽
    TILE_RED_KEY = 2,    // 빨간 열쇠
    TILE_BLUE_KEY = 3,    // 파란 열쇠
    TILE_YELLOW_KEY = 4,    // 노란 열쇠
    TILE_PANEL_WALL = 5,    // 🔹 특별한 장식 벽 (여기 추가)
    TILE_LAMP = 6      // 전등
};


// 미로 로딩 함수
bool loadMaze(const char* filename);
void getStartPosition(float& x, float& z);
void getGoalPosition(float& x, float& z);

bool initMazeTextures();
void setMazeTexture(int idx);
void nextMazeTexture();

// 미로 렌더링 함수
void drawMaze();
void drawWall();
void drawFloorTile();
void drawRedKey();
void drawBlueKey();
void drawYellowKey();
void drawGoal(bool activated);

void drawLamp();
void setupLamps();

void applyMaterial(const Material& m);

bool initWallShader();

#endif