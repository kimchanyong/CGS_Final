#include "maze.h"
#include <GL/glut.h>
#include <cmath>

void drawWall() {
    GLfloat wall_ambient[] = { 0.3f, 0.3f, 0.3f, 1.0f };
    GLfloat wall_diffuse[] = { 0.5f, 0.5f, 0.5f, 1.0f };
    GLfloat wall_specular[] = { 0.2f, 0.2f, 0.2f, 1.0f };

    glMaterialfv(GL_FRONT, GL_AMBIENT, wall_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, wall_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, wall_specular);
    glMaterialf(GL_FRONT, GL_SHININESS, 10.0f);

    float w = CELL_SIZE / 2.0f;
    float h = WALL_HEIGHT;

    // 벽 면 그리기
    glBegin(GL_QUADS);

    // 앞면
    glNormal3f(0, 0, 1);
    glVertex3f(-w, 0, w);
    glVertex3f(w, 0, w);
    glVertex3f(w, h, w);
    glVertex3f(-w, h, w);

    // 뒷면
    glNormal3f(0, 0, -1);
    glVertex3f(-w, 0, -w);
    glVertex3f(-w, h, -w);
    glVertex3f(w, h, -w);
    glVertex3f(w, 0, -w);

    // 왼쪽면
    glNormal3f(-1, 0, 0);
    glVertex3f(-w, 0, -w);
    glVertex3f(-w, 0, w);
    glVertex3f(-w, h, w);
    glVertex3f(-w, h, -w);

    // 오른쪽면
    glNormal3f(1, 0, 0);
    glVertex3f(w, 0, -w);
    glVertex3f(w, h, -w);
    glVertex3f(w, h, w);
    glVertex3f(w, 0, w);

    // 윗면
    glNormal3f(0, 1, 0);
    glVertex3f(-w, h, -w);
    glVertex3f(-w, h, w);
    glVertex3f(w, h, w);
    glVertex3f(w, h, -w);

    // 아랫면
    glNormal3f(0, -1, 0);
    glVertex3f(-w, 0, -w);
    glVertex3f(w, 0, -w);
    glVertex3f(w, 0, w);
    glVertex3f(-w, 0, w);

    glEnd();

    // 모서리 강조선 그리기
    glDisable(GL_LIGHTING);
    glColor3f(0.1f, 0.1f, 0.15f);
    glLineWidth(2.0f);

    // 수직 모서리 (4개)
    glBegin(GL_LINES);
    glVertex3f(-w, 0, w);
    glVertex3f(-w, h, w);
    glVertex3f(w, 0, w);
    glVertex3f(w, h, w);
    glVertex3f(-w, 0, -w);
    glVertex3f(-w, h, -w);
    glVertex3f(w, 0, -w);
    glVertex3f(w, h, -w);
    glEnd();

    // 아래쪽 수평 모서리
    glBegin(GL_LINE_LOOP);
    glVertex3f(-w, 0, -w);
    glVertex3f(w, 0, -w);
    glVertex3f(w, 0, w);
    glVertex3f(-w, 0, w);
    glEnd();

    // 위쪽 수평 모서리
    glBegin(GL_LINE_LOOP);
    glVertex3f(-w, h, -w);
    glVertex3f(w, h, -w);
    glVertex3f(w, h, w);
    glVertex3f(-w, h, w);
    glEnd();

    glEnable(GL_LIGHTING);
    glLineWidth(1.0f);
}

void drawFloorTile() {
    GLfloat floor_ambient[] = { 0.2f, 0.2f, 0.25f, 1.0f };
    GLfloat floor_diffuse[] = { 0.4f, 0.4f, 0.5f, 1.0f };

    glMaterialfv(GL_FRONT, GL_AMBIENT, floor_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, floor_diffuse);

    float w = CELL_SIZE / 2.0f;

    glBegin(GL_QUADS);
    glNormal3f(0, 1, 0);
    glVertex3f(-w, 0, -w);
    glVertex3f(-w, 0, w);
    glVertex3f(w, 0, w);
    glVertex3f(w, 0, -w);
    glEnd();
}

void drawItem() {
    drawFloorTile();

    GLfloat item_ambient[] = { 0.5f, 0.4f, 0.0f, 1.0f };
    GLfloat item_diffuse[] = { 0.8f, 0.7f, 0.0f, 1.0f };
    GLfloat item_specular[] = { 1.0f, 1.0f, 0.5f, 1.0f };

    glMaterialfv(GL_FRONT, GL_AMBIENT, item_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, item_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, item_specular);
    glMaterialf(GL_FRONT, GL_SHININESS, 50.0f);

    glPushMatrix();
    glTranslatef(0, 0.5f, 0);
    glScalef(0.4f, 0.4f, 0.4f);
    glutSolidCube(1.0);
    glPopMatrix();
}

void drawGoal() {
    drawFloorTile();

    GLfloat goal_ambient[] = { 0.0f, 0.3f, 0.0f, 1.0f };
    GLfloat goal_diffuse[] = { 0.0f, 0.8f, 0.0f, 1.0f };
    GLfloat goal_specular[] = { 0.2f, 1.0f, 0.2f, 1.0f };

    glMaterialfv(GL_FRONT, GL_AMBIENT, goal_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, goal_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, goal_specular);
    glMaterialf(GL_FRONT, GL_SHININESS, 30.0f);

    float radius = 0.5f;
    float height = 1.0f;
    int segments = 20;

    // 원기둥 옆면
    glBegin(GL_QUAD_STRIP);
    for (int i = 0; i <= segments; i++) {
        float angle = (2.0f * 3.14159265f * i) / segments;
        float x = radius * std::cos(angle);
        float z = radius * std::sin(angle);

        glNormal3f(x / radius, 0, z / radius);
        glVertex3f(x, 0, z);
        glVertex3f(x, height, z);
    }
    glEnd();

    // 윗면
    glBegin(GL_TRIANGLE_FAN);
    glNormal3f(0, 1, 0);
    glVertex3f(0, height, 0);
    for (int i = 0; i <= segments; i++) {
        float angle = (2.0f * 3.14159265f * i) / segments;
        float x = radius * std::cos(angle);
        float z = radius * std::sin(angle);
        glVertex3f(x, height, z);
    }
    glEnd();
}

void drawMaze() {
    for (int i = 0; i < MAZE_ROWS; i++) {
        for (int j = 0; j < MAZE_COLS; j++) {
            float x = i * CELL_SIZE - (MAZE_ROWS * CELL_SIZE) / 2.0f;
            float z = j * CELL_SIZE - (MAZE_COLS * CELL_SIZE) / 2.0f;

            glPushMatrix();
            glTranslatef(x, 0, z);

            switch (MAZE[i][j]) {
            case TILE_WALL:
                drawWall();
                break;
            case TILE_ITEM:
                drawItem();
                break;
            case TILE_GOAL:
                drawGoal();
                break;
            case TILE_START:  // -1은 바닥으로 렌더링
                drawFloorTile();
                break;
            default:
                drawFloorTile();
                break;
            }

            glPopMatrix();
        }
    }
}