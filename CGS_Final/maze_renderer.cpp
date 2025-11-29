#include "object.h"
#include "maze.h"
#include "player.h"
#include <GL/glut.h>
#include <cmath>
#include <vector>
#include <iostream>
#include <direct.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

static std::vector<GLuint> g_mazeTextures;
static int g_currentMazeTex = -1;


GLuint loadTextureFromFile(const char* filename)
{
    int width, height, channels;

    stbi_set_flip_vertically_on_load(true);

    unsigned char* data = stbi_load(filename, &width, &height, &channels, 0);
    if (!data) {
        std::cerr << "Failed to load texture image: " << filename << std::endl;
        return 0;
    }

    std::cout << "Loaded texture: " << filename
        << " (" << width << "x" << height
        << ", channels=" << channels << ")\n";

    GLenum format;
    if (channels == 3) {
        format = GL_RGB;
    }
    else if (channels == 4) {
        format = GL_RGBA;
    }
    else {
        std::cerr << "Unsupported channel count (" << channels
            << ") in texture: " << filename << std::endl;
        stbi_image_free(data);
        return 0;
    }

    GLuint texId;
    glGenTextures(1, &texId);
    glBindTexture(GL_TEXTURE_2D, texId);

    // 🔹 mipmap 안 쓰는 일반 필터 (이게 중요!)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        format,           // 내부 포맷
        width,
        height,
        0,
        format,           // 입력 데이터 포맷
        GL_UNSIGNED_BYTE,
        data
    );

    stbi_image_free(data);
    return texId;
}

bool initMazeTextures() {
    g_mazeTextures.clear();
    g_mazeTextures.push_back(loadTextureFromFile("textures/wall1.jpg"));
    g_mazeTextures.push_back(loadTextureFromFile("textures/wall2.jpg"));
    g_mazeTextures.push_back(loadTextureFromFile("textures/wall3.jpg"));

    for (int i = 0; i < (int)g_mazeTextures.size(); ++i) {
        if (g_mazeTextures[i] == 0) {
            std::cerr << "Texture " << i << " failed to load." << std::endl;
            return false;
        }
    }

    g_currentMazeTex = 0;
    std::cout << "Maze textures initialized (image files). Count: "
        << g_mazeTextures.size() << std::endl;
    return true;
}


void setMazeTexture(int idx) {
    if (idx < 0 || idx >= (int)g_mazeTextures.size()) return;
    g_currentMazeTex = idx;
}

void nextMazeTexture() {
    if (g_mazeTextures.empty()) return;
    g_currentMazeTex = (g_currentMazeTex + 1) % g_mazeTextures.size();
}


Mesh goblet1Mesh;
Mesh goblet2Mesh;
Mesh goblet3Mesh;

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

    if (g_currentMazeTex >= 0 && g_currentMazeTex < (int)g_mazeTextures.size()) {
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, g_mazeTextures[g_currentMazeTex]);
    }
    else {
        glDisable(GL_TEXTURE_2D);
    }


    glBegin(GL_QUADS);

    // 앞면
    glNormal3f(0, 0, 1);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-w, 0, w);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(w, 0, w);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(w, h, w);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-w, h, w);

    // 뒷면
    glNormal3f(0, 0, -1);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-w, 0, -w);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(w, 0, -w);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(w, h, -w);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-w, h, -w);

    // 왼쪽면
    glNormal3f(-1, 0, 0);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-w, 0, -w);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(-w, 0, w);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(-w, h, w);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-w, h, -w);

    // 오른쪽면
    glNormal3f(1, 0, 0);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(w, 0, -w);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(w, 0, w);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(w, h, w);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(w, h, -w);

    // 윗면
    glNormal3f(0, 1, 0);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-w, h, -w);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(w, h, -w);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(w, h, w);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-w, h, w);

    // 아랫면
    glNormal3f(0, -1, 0);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-w, 0, -w);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(w, 0, -w);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(w, 0, w);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-w, 0, w);

    glEnd();

    glDisable(GL_TEXTURE_2D);

    // 모서리 강조선
    glDisable(GL_LIGHTING);
    glColor3f(0.1f, 0.1f, 0.15f);
    glLineWidth(2.0f);

    glBegin(GL_LINES);
    glVertex3f(-w, 0, w); glVertex3f(-w, h, w);
    glVertex3f(w, 0, w); glVertex3f(w, h, w);
    glVertex3f(-w, 0, -w); glVertex3f(-w, h, -w);
    glVertex3f(w, 0, -w); glVertex3f(w, h, -w);
    glEnd();

    glBegin(GL_LINE_LOOP);
    glVertex3f(-w, 0, -w); glVertex3f(w, 0, -w);
    glVertex3f(w, 0, w); glVertex3f(-w, 0, w);
    glEnd();

    glBegin(GL_LINE_LOOP);
    glVertex3f(-w, h, -w); glVertex3f(w, h, -w);
    glVertex3f(w, h, w); glVertex3f(-w, h, w);
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

void drawRedKey() {
    drawFloorTile();

    // 빨간색 열쇠
    GLfloat key_ambient[] = { 0.5f, 0.0f, 0.0f, 1.0f };
    GLfloat key_diffuse[] = { 0.9f, 0.1f, 0.1f, 1.0f };
    GLfloat key_specular[] = { 1.0f, 0.3f, 0.3f, 1.0f };

    glMaterialfv(GL_FRONT, GL_AMBIENT, key_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, key_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, key_specular);
    glMaterialf(GL_FRONT, GL_SHININESS, 50.0f);

    glPushMatrix();
    glTranslatef(0, 0.5f, 0);
    glScalef(0.4f, 0.4f, 0.4f);
    goblet1Mesh.Draw();
    glPopMatrix();
}

void drawBlueKey() {
    drawFloorTile();

    // 파란색 열쇠
    GLfloat key_ambient[] = { 0.0f, 0.0f, 0.5f, 1.0f };
    GLfloat key_diffuse[] = { 0.1f, 0.1f, 0.9f, 1.0f };
    GLfloat key_specular[] = { 0.3f, 0.3f, 1.0f, 1.0f };

    glMaterialfv(GL_FRONT, GL_AMBIENT, key_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, key_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, key_specular);
    glMaterialf(GL_FRONT, GL_SHININESS, 50.0f);

    glPushMatrix();
    glTranslatef(0, 0.5f, 0);
    glScalef(0.4f, 0.4f, 0.4f);
    goblet2Mesh.Draw();
    glPopMatrix();
}

void drawYellowKey() {
    drawFloorTile();

    // 노란색 열쇠
    GLfloat key_ambient[] = { 0.5f, 0.5f, 0.0f, 1.0f };
    GLfloat key_diffuse[] = { 0.9f, 0.9f, 0.1f, 1.0f };
    GLfloat key_specular[] = { 1.0f, 1.0f, 0.3f, 1.0f };

    glMaterialfv(GL_FRONT, GL_AMBIENT, key_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, key_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, key_specular);
    glMaterialf(GL_FRONT, GL_SHININESS, 50.0f);

    glPushMatrix();
    glTranslatef(0, 0.5f, 0);
    glScalef(0.4f, 0.4f, 0.4f);
    goblet3Mesh.Draw();
    glPopMatrix();
}

void drawGoal(bool activated) {
    drawFloorTile();

    // 활성화 상태에 따라 색상 변경
    GLfloat goal_ambient[4], goal_diffuse[4], goal_specular[4];

    if (activated) {
        // 활성화: 밝은 초록색
        goal_ambient[0] = 0.0f; goal_ambient[1] = 0.5f; goal_ambient[2] = 0.0f; goal_ambient[3] = 1.0f;
        goal_diffuse[0] = 0.0f; goal_diffuse[1] = 1.0f; goal_diffuse[2] = 0.0f; goal_diffuse[3] = 1.0f;
        goal_specular[0] = 0.3f; goal_specular[1] = 1.0f; goal_specular[2] = 0.3f; goal_specular[3] = 1.0f;
    }
    else {
        // 비활성화: 어두운 회색
        goal_ambient[0] = 0.2f; goal_ambient[1] = 0.2f; goal_ambient[2] = 0.2f; goal_ambient[3] = 1.0f;
        goal_diffuse[0] = 0.3f; goal_diffuse[1] = 0.3f; goal_diffuse[2] = 0.3f; goal_diffuse[3] = 1.0f;
        goal_specular[0] = 0.1f; goal_specular[1] = 0.1f; goal_specular[2] = 0.1f; goal_specular[3] = 1.0f;
    }

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
            case TILE_RED_KEY:
                drawRedKey();
                break;
            case TILE_BLUE_KEY:
                drawBlueKey();
                break;
            case TILE_YELLOW_KEY:
                drawYellowKey();
                break;
            case TILE_GOAL:
                drawGoal(player.goalActivated);  // 활성화 상태 전달
                break;
            case TILE_START:
            case TILE_EMPTY:
            default:
                drawFloorTile();
                break;
            }

            glPopMatrix();
        }
    }
}