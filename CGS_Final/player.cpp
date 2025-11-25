#include "player.h"
#include "collision.h"
#include "maze.h"
#include "tagger.h"  // 술래 체크를 위해 추가
#include <GL/glut.h>
#include <iostream>
#include <cmath>

Player player;
bool keyStates[256] = { false };

int lastMouseX = 640;
int lastMouseY = 360;
bool firstMouse = true;

void initPlayer() {
    getStartPosition(player.x, player.z);

    player.y = PLAYER_HEIGHT / 2.0f;
    player.angleY = 0.0f;
    player.angleX = 0.0f;
    player.speed = MOVE_SPEED;

    player.hasRedKey = false;
    player.hasBlueKey = false;
    player.hasYellowKey = false;

    player.reachedGoal = false;
    player.goalActivated = false;
    player.isCaught = false;  // 술래에게 잡힘 상태 초기화

    std::cout << "Player initialized at world position (" << player.x << ", " << player.z << ")" << std::endl;
}

bool hasAllKeys() {
    return player.hasRedKey && player.hasBlueKey && player.hasYellowKey;
}

int getKeyCount() {
    return (player.hasRedKey ? 1 : 0) + (player.hasBlueKey ? 1 : 0) + (player.hasYellowKey ? 1 : 0);
}

void updatePlayer(float deltaTime) {
    // 게임 종료 상태면 이동 불가
    if (player.reachedGoal || player.isCaught) return;

    float moveX = 0.0f;
    float moveZ = 0.0f;

    if (keyStates['w'] || keyStates['W']) {
        moveZ -= 1.0f;
    }
    if (keyStates['s'] || keyStates['S']) {
        moveZ += 1.0f;
    }
    if (keyStates['a'] || keyStates['A']) {
        moveX -= 1.0f;
    }
    if (keyStates['d'] || keyStates['D']) {
        moveX += 1.0f;
    }

    normalize2D(moveX, moveZ);

    float angleRad = player.angleY * 3.14159265f / 180.0f;
    float cosAngle = std::cos(angleRad);
    float sinAngle = std::sin(angleRad);

    float worldMoveX = moveX * cosAngle - moveZ * sinAngle;
    float worldMoveZ = moveX * sinAngle + moveZ * cosAngle;

    float nx = player.x + worldMoveX * player.speed;
    float nz = player.z + worldMoveZ * player.speed;

    if (!checkCollision(nx, nz, PLAYER_RADIUS)) {
        player.x = nx;
        player.z = nz;
    }
    else {
        if (!checkCollision(nx, player.z, PLAYER_RADIUS)) {
            player.x = nx;
        }
        else if (!checkCollision(player.x, nz, PLAYER_RADIUS)) {
            player.z = nz;
        }
    }

    // 열쇠 수집
    KeyCollection keys = checkKeyCollision(player.x, player.z, PLAYER_RADIUS);

    if (keys.redKey && !player.hasRedKey) {
        player.hasRedKey = true;
        std::cout << "Keys: " << getKeyCount() << "/3" << std::endl;
    }
    if (keys.blueKey && !player.hasBlueKey) {
        player.hasBlueKey = true;
        std::cout << "Keys: " << getKeyCount() << "/3" << std::endl;
    }
    if (keys.yellowKey && !player.hasYellowKey) {
        player.hasYellowKey = true;
        std::cout << "Keys: " << getKeyCount() << "/3" << std::endl;
    }

    if (hasAllKeys() && !player.goalActivated) {
        player.goalActivated = true;
        std::cout << "\n========================================" << std::endl;
        std::cout << "  출구가 활성화되었습니다!" << std::endl;
        std::cout << "  The exit is now activated!" << std::endl;
        std::cout << "========================================\n" << std::endl;
    }

    // 골 도달 체크
    if (checkGoalCollision(player.x, player.z, PLAYER_RADIUS)) {
        if (player.goalActivated) {
            if (!player.reachedGoal) {
                player.reachedGoal = true;
                std::cout << "\n========================================" << std::endl;
                std::cout << "  축하합니다! 탈출에 성공했습니다!" << std::endl;
                std::cout << "  CONGRATULATIONS! You escaped!" << std::endl;
                std::cout << "  Press ESC to exit" << std::endl;
                std::cout << "========================================\n" << std::endl;
            }
        }
        else {
            static bool showedWarning = false;
            if (!showedWarning) {
                std::cout << "\n출구가 잠겨있습니다! 모든 열쇠를 찾으세요! ("
                    << getKeyCount() << "/3)" << std::endl;
                showedWarning = true;
            }
        }
    }

    // 술래 충돌 체크
    if (checkTaggerCatch(player.x, player.z)) {
        if (!player.isCaught) {
            player.isCaught = true;
            std::cout << "\n========================================" << std::endl;
            std::cout << "  GAME OVER!" << std::endl;
            std::cout << "  술래에게 잡혔습니다!" << std::endl;
            std::cout << "  You were caught by the tagger!" << std::endl;
            std::cout << "  Press ESC to exit" << std::endl;
            std::cout << "========================================\n" << std::endl;
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
    gluPerspective(70.0, 16.0 / 9.0, 0.1, 200.0);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    float camX = player.x;
    float camY = player.y + PLAYER_HEIGHT / 2.0f;
    float camZ = player.z;

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

    if (key == 27) {
        std::cout << "Exiting..." << std::endl;
        exit(0);
    }
}

void onKeyRelease(unsigned char key, int x, int y) {
    keyStates[key] = false;
}

void onSpecialKeyPress(int key, int x, int y) {
    switch (key) {
    case GLUT_KEY_UP:
        keyStates['w'] = true;
        break;
    case GLUT_KEY_DOWN:
        keyStates['s'] = true;
        break;
    case GLUT_KEY_LEFT:
        keyStates['a'] = true;
        break;
    case GLUT_KEY_RIGHT:
        keyStates['d'] = true;
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

void onMouseMove(int x, int y) {
    // 🔒 게임오버 또는 탈출 후에는 마우스 시점 이동 차단
    if (player.isCaught || player.reachedGoal) {
        return;
    }

    if (firstMouse) {
        lastMouseX = x;
        lastMouseY = y;
        firstMouse = false;
        return;
    }

    float xOffset = (x - lastMouseX) * MOUSE_SENSITIVITY;
    float yOffset = (lastMouseY - y) * MOUSE_SENSITIVITY;

    lastMouseX = x;
    lastMouseY = y;

    player.angleY += xOffset;
    player.angleX += yOffset;

    if (player.angleX > 89.0f)  player.angleX = 89.0f;
    if (player.angleX < -89.0f) player.angleX = -89.0f;

    if (player.angleY > 360.0f) player.angleY -= 360.0f;
    if (player.angleY < 0.0f)   player.angleY += 360.0f;
}
