#include "player.h"
#include "collision.h"
#include "maze.h"
#include <GL/glut.h>
#include <iostream>
#include <cmath>

// 전역 변수
Player player;
bool keyStates[256] = { false };

// 마우스 상태
int lastMouseX = 640;
int lastMouseY = 360;
bool firstMouse = true;

void initPlayer() {
    player.x = 0.0f;
    player.y = PLAYER_HEIGHT / 2.0f;
    player.z = 0.0f;
    player.angleY = 0.0f;
    player.angleX = 0.0f;  // 상하 각도 초기화
    player.speed = MOVE_SPEED;
    player.itemsCollected = 0;
    player.totalItems = 0;
    player.reachedGoal = false;

    std::cout << "Player initialized at (" << player.x << ", " << player.z << ")" << std::endl;
}

void updatePlayer(float deltaTime) {
    if (player.reachedGoal) return;

    float moveX = 0.0f;
    float moveZ = 0.0f;

    // 키 상태에 따른 이동 방향 계산
    // W/S: 앞/뒤 (카메라 바라보는 방향 기준)
    if (keyStates['w'] || keyStates['W']) {
        moveZ -= 1.0f;  // 전진
    }
    if (keyStates['s'] || keyStates['S']) {
        moveZ += 1.0f;  // 후진
    }

    // A/D: 좌/우 (카메라 기준 좌우 이동)
    if (keyStates['a'] || keyStates['A']) {
        moveX -= 1.0f;  // 왼쪽
    }
    if (keyStates['d'] || keyStates['D']) {
        moveX += 1.0f;  // 오른쪽
    }

    // 대각선 이동 정규화
    normalize2D(moveX, moveZ);

    // 카메라 방향 기준으로 이동 벡터 변환
    float angleRad = player.angleY * 3.14159265f / 180.0f;
    float cosAngle = std::cos(angleRad);
    float sinAngle = std::sin(angleRad);

    // 회전 행렬 적용
    float worldMoveX = moveX * cosAngle - moveZ * sinAngle;
    float worldMoveZ = moveX * sinAngle + moveZ * cosAngle;

    // 실제 이동
    float nx = player.x + worldMoveX * player.speed;
    float nz = player.z + worldMoveZ * player.speed;

    // 충돌 검사
    if (!checkCollision(nx, nz, PLAYER_RADIUS)) {
        player.x = nx;
        player.z = nz;
    }
    else {
        // 슬라이딩 충돌
        if (!checkCollision(nx, player.z, PLAYER_RADIUS)) {
            player.x = nx;
        }
        else if (!checkCollision(player.x, nz, PLAYER_RADIUS)) {
            player.z = nz;
        }
    }

    // 아이템 수집
    int collected = checkItemCollision(player.x, player.z, PLAYER_RADIUS);
    player.itemsCollected += collected;

    if (collected > 0) {
        std::cout << "Items: " << player.itemsCollected << "/" << player.totalItems << std::endl;
    }

    // 골 도달 체크
    if (player.itemsCollected >= player.totalItems) {
        if (checkGoalCollision(player.x, player.z, PLAYER_RADIUS)) {
            if (!player.reachedGoal) {
                player.reachedGoal = true;
                std::cout << "\n=== GOAL REACHED! ===" << std::endl;
                std::cout << "All items collected: " << player.itemsCollected << "/" << player.totalItems << std::endl;
                std::cout << "Press ESC to exit" << std::endl;
            }
        }
    }
    else {
        static bool showedMessage = false;
        if (checkGoalCollision(player.x, player.z, PLAYER_RADIUS) && !showedMessage) {
            std::cout << "Collect all items first! ("
                << player.itemsCollected << "/" << player.totalItems << ")" << std::endl;
            showedMessage = true;
        }
        if (!checkGoalCollision(player.x, player.z, PLAYER_RADIUS)) {
            showedMessage = false;
        }
    }
}

void drawPlayer() {
    GLfloat player_ambient[] = { 0.2f, 0.2f, 0.8f, 1.0f };
    GLfloat player_diffuse[] = { 0.3f, 0.3f, 1.0f, 1.0f };
    GLfloat player_specular[] = { 0.5f, 0.5f, 1.0f, 1.0f };

    glMaterialfv(GL_FRONT, GL_AMBIENT, player_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, player_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, player_specular);
    glMaterialf(GL_FRONT, GL_SHININESS, 50.0f);

    glPushMatrix();
    glTranslatef(player.x, player.y, player.z);
    glutSolidSphere(PLAYER_RADIUS, 16, 16);
    glPopMatrix();
}

void setupCamera() {
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(70.0, 16.0 / 9.0, 0.1, 200.0);  // 시야각 증가, 렌더 거리 증가

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // 1인칭 카메라
    float camX = player.x;
    float camY = player.y + PLAYER_HEIGHT / 2.0f;
    float camZ = player.z;

    // 카메라가 바라보는 방향 계산 (angleX, angleY 모두 적용)
    float angleYRad = player.angleY * 3.14159265f / 180.0f;
    float angleXRad = player.angleX * 3.14159265f / 180.0f;

    float lookX = camX + std::sin(angleYRad) * std::cos(angleXRad);
    float lookY = camY + std::sin(angleXRad);
    float lookZ = camZ - std::cos(angleYRad) * std::cos(angleXRad);

    gluLookAt(
        camX, camY, camZ,
        lookX, lookY, lookZ,
        0, 1, 0
    );
}

void onKeyPress(unsigned char key, int x, int y) {
    keyStates[key] = true;

    if (key == 27) {  // ESC
        std::cout << "Exiting..." << std::endl;
        exit(0);
    }
}

void onKeyRelease(unsigned char key, int x, int y) {
    keyStates[key] = false;
}

void onSpecialKeyPress(int key, int x, int y) {
    // 방향키 수정: 올바른 방향으로 매핑
    switch (key) {
    case GLUT_KEY_UP:
        keyStates['w'] = true;  // 위 = 전진
        break;
    case GLUT_KEY_DOWN:
        keyStates['s'] = true;  // 아래 = 후진
        break;
    case GLUT_KEY_LEFT:
        keyStates['a'] = true;  // 왼쪽 = 왼쪽 이동
        break;
    case GLUT_KEY_RIGHT:
        keyStates['d'] = true;  // 오른쪽 = 오른쪽 이동
        break;
    }
}

void onSpecialKeyRelease(int key, int x, int y) {
    switch (key) {
    case GLUT_KEY_UP:
        keyStates['w'] = false;
        break;
    case GLUT_KEY_DOWN:
        keyStates['s'] = false;
        break;
    case GLUT_KEY_LEFT:
        keyStates['a'] = false;
        break;
    case GLUT_KEY_RIGHT:
        keyStates['d'] = false;
        break;
    }
}

// 마우스 이동 콜백 - 부드러운 시점 회전
void onMouseMove(int x, int y) {
    if (firstMouse) {
        lastMouseX = x;
        lastMouseY = y;
        firstMouse = false;
        return;
    }

    // 마우스 이동량 계산
    float xOffset = (x - lastMouseX) * MOUSE_SENSITIVITY;
    float yOffset = (lastMouseY - y) * MOUSE_SENSITIVITY;  // Y축은 반대

    lastMouseX = x;
    lastMouseY = y;

    // 각도 업데이트
    player.angleY += xOffset;
    player.angleX += yOffset;

    // 상하 각도 제한 (-89도 ~ 89도)
    if (player.angleX > 89.0f) player.angleX = 89.0f;
    if (player.angleX < -89.0f) player.angleX = -89.0f;

    // 좌우 각도는 0~360도로 정규화
    if (player.angleY > 360.0f) player.angleY -= 360.0f;
    if (player.angleY < 0.0f) player.angleY += 360.0f;
}