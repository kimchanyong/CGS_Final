#ifndef PLAYER_H
#define PLAYER_H

#include "util.h"

// 플레이어 상태 구조체
struct Player {
    float x, y, z;          // 위치
    float angleY;           // Y축 회전 (좌우)
    float angleX;           // X축 회전 (상하)
    float speed;            // 이동 속도
    int itemsCollected;     // 수집한 아이템 수
    int totalItems;         // 전체 아이템 수
    bool reachedGoal;       // 골 도달 여부
};

// 전역 플레이어 객체
extern Player player;

// 키 상태 배열
extern bool keyStates[256];

// 마우스 상태
extern int lastMouseX;
extern int lastMouseY;
extern bool firstMouse;

// 플레이어 관련 함수
void initPlayer();
void updatePlayer(float deltaTime);
void drawPlayer();

// 카메라 설정
void setupCamera();

// 입력 처리 콜백
void onKeyPress(unsigned char key, int x, int y);
void onKeyRelease(unsigned char key, int x, int y);
void onSpecialKeyPress(int key, int x, int y);
void onSpecialKeyRelease(int key, int x, int y);
void onMouseMove(int x, int y);

#endif