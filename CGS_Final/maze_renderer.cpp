#include <GL/glew.h> 
#include <GL/glut.h>

#include "maze.h"
#include "model.h"
#include "player.h"

#include <cmath>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <direct.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

static std::vector<GLuint> g_mazeTextures;
static int g_currentMazeTex = -1;

GLuint g_panelDiffuseTex = 0;
GLuint g_wallDiffuseTex = 0;  
GLuint g_wallNormalTex = 0;   
GLuint g_wallHeightTex = 0;   

GLuint g_floorTex = 0;

GLuint loadTextureFromFile(const char* filename);

GLuint g_wallShader = 0;

GLuint g_redKeyTex = 0;
GLuint g_blueKeyTex = 0;
GLuint g_yellowKeyTex = 0;

ObjMesh g_redKeyMesh;
ObjMesh g_blueKeyMesh;
ObjMesh g_yellowKeyMesh;
bool g_redKeyLoaded = false;
bool g_blueKeyLoaded = false;
bool g_yellowKeyLoaded = false;

void initModels()
{
    if (loadOBJ("models/key.obj", g_redKeyMesh))
        g_redKeyLoaded = true;
    else
        std::cerr << "Failed to load red_key.obj\n";

    if (loadOBJ("models/key.obj", g_blueKeyMesh))
        g_blueKeyLoaded = true;
    else
        std::cerr << "Failed to load blue_key.obj\n";

    if (loadOBJ("models/key.obj", g_yellowKeyMesh))
        g_yellowKeyLoaded = true;
    else
        std::cerr << "Failed to load yellow_key.obj\n";

    g_redKeyTex = loadTextureFromFile("textures/key_red.png");
    g_blueKeyTex = loadTextureFromFile("textures/key_blue.png");
    g_yellowKeyTex = loadTextureFromFile("textures/key_yellow.jpg");

    if (!g_redKeyTex)    std::cerr << "Failed to load key_red.png\n";
    if (!g_blueKeyTex)   std::cerr << "Failed to load key_blue.png\n";
    if (!g_yellowKeyTex) std::cerr << "Failed to load key_yellow.png\n";
}

Material g_wallMaterials[3] = {
    {
        {0.2f, 0.2f, 0.25f, 1.0f},   // ambient
        {0.6f, 0.6f, 0.7f, 1.0f},    // diffuse
        {0.2f, 0.2f, 0.3f, 1.0f},    // specular
        20.0f
    },
    {
        {0.25f, 0.15f, 0.15f, 1.0f},
        {0.8f,  0.3f,  0.3f,  1.0f},
        {0.3f,  0.1f,  0.1f,  1.0f},
        10.0f
    },
    {
        {0.15f, 0.15f, 0.18f, 1.0f},
        {0.5f,  0.5f,  0.6f,  1.0f},
        {0.8f,  0.8f,  0.9f,  1.0f},
        80.0f
    }
};

int g_currentWallMaterial = 0;

void applyMaterial(const Material& m)
{
    glMaterialfv(GL_FRONT, GL_AMBIENT, m.ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, m.diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, m.specular);
    glMaterialf(GL_FRONT, GL_SHININESS, m.shininess);
}


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

    g_wallDiffuseTex = loadTextureFromFile("textures/panel_diffuse.png");
    if (g_wallDiffuseTex == 0) {
        std::cerr << "Failed to load panel_diffuse.png\n";
        return false;
    }

    g_wallNormalTex = loadTextureFromFile("textures/panel_normal.png");
    if (g_wallNormalTex == 0) {
        std::cerr << "Failed to load panel_normal.png\n";
        return false;
    }

    g_wallHeightTex = loadTextureFromFile("textures/panel_height.png");
    if (g_wallHeightTex == 0) {
        std::cerr << "Failed to load panel_height.png\n";
        return false;
    }

    g_floorTex = loadTextureFromFile("textures/floor_01.png");  
    if (g_floorTex == 0) {
        std::cerr << "Failed to load floor.png\n";
        return false;
    }

    g_panelDiffuseTex = g_wallDiffuseTex;

    std::cout << "Wall textures (diffuse/normal/height) loaded.\n";
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


void drawWall() {
    float w = CELL_SIZE / 2.0f;
    float h = WALL_HEIGHT;

    glEnable(GL_LIGHTING);
    applyMaterial(g_wallMaterials[g_currentWallMaterial]);

    if (g_wallShader == 0) {
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, g_wallDiffuseTex);

        glBegin(GL_QUADS);
        glNormal3f(0, 0, 1);
        glTexCoord2f(0.0f, 0.0f); glVertex3f(-w, 0, w);
        glTexCoord2f(1.0f, 0.0f); glVertex3f(w, 0, w);
        glTexCoord2f(1.0f, 1.0f); glVertex3f(w, h, w);
        glTexCoord2f(0.0f, 1.0f); glVertex3f(-w, h, w);
        glEnd();

        glDisable(GL_TEXTURE_2D);
        return;
    }

    glUseProgram(g_wallShader);

    glActiveTexture(GL_TEXTURE0);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, g_wallDiffuseTex);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, g_wallNormalTex);

    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, g_wallHeightTex);

    GLint locDiffuse = glGetUniformLocation(g_wallShader, "uDiffuseMap");
    GLint locNormal = glGetUniformLocation(g_wallShader, "uNormalMap");
    GLint locHeight = glGetUniformLocation(g_wallShader, "uHeightMap");
    GLint locHeightScale = glGetUniformLocation(g_wallShader, "uHeightScale");

    if (locDiffuse >= 0)     glUniform1i(locDiffuse, 0);
    if (locNormal >= 0)     glUniform1i(locNormal, 1);
    if (locHeight >= 0)     glUniform1i(locHeight, 2);
    if (locHeightScale >= 0) glUniform1f(locHeightScale, 0.05f);

    glBegin(GL_QUADS);

    glNormal3f(0, 0, 1);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-w, 0, w);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(w, 0, w);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(w, h, w);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-w, h, w);

    glNormal3f(0, 0, -1);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-w, 0, -w);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(w, 0, -w);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(w, h, -w);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-w, h, -w);


    glNormal3f(-1, 0, 0);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-w, 0, -w);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(-w, 0, w);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(-w, h, w);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-w, h, -w);

    glNormal3f(1, 0, 0);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(w, 0, -w);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(w, 0, w);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(w, h, w);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(w, h, -w);

    glNormal3f(0, 1, 0);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-w, h, -w);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(w, h, -w);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(w, h, w);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-w, h, w);

    glNormal3f(0, -1, 0);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-w, 0, -w);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(w, 0, -w);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(w, 0, w);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-w, 0, w);

    glEnd();

    glUseProgram(0);

    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, 0);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, 0);
}


void drawPanelWall() {
    float w = CELL_SIZE / 2.0f;
    float h = WALL_HEIGHT;

    glEnable(GL_LIGHTING);
    applyMaterial(g_wallMaterials[g_currentWallMaterial]);

    if (g_panelDiffuseTex != 0) {
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, g_panelDiffuseTex);
    }
    else {
        glDisable(GL_TEXTURE_2D);
    }

    glColor3f(1.0f, 1.0f, 1.0f);

    glBegin(GL_QUADS);
    glNormal3f(0, 0, 1);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-w, 0, w);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(w, 0, w);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(w, h, w);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-w, h, w);
    glEnd();

    glDisable(GL_TEXTURE_2D);
}

void drawFloorTile() {
    GLfloat floor_ambient[] = { 0.25f, 0.25f, 0.27f, 1.0f };
    GLfloat floor_diffuse[] = { 0.9f,  0.9f,  0.95f, 1.0f };
    GLfloat floor_specular[] = { 0.9f,  0.9f,  0.9f,  1.0f };

    glMaterialfv(GL_FRONT, GL_AMBIENT, floor_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, floor_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, floor_specular);
    glMaterialf(GL_FRONT, GL_SHININESS, 80.0f);

    float w = CELL_SIZE / 2.0f;

    if (g_floorTex != 0) {
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, g_floorTex);
    }

    glColor3f(1.0f, 1.0f, 1.0f); 

    glBegin(GL_QUADS);
    glNormal3f(0, 1, 0);

    glTexCoord2f(0.0f, 0.0f); glVertex3f(-w, 0, -w);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(w, 0, -w);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(w, 0, w);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-w, 0, w);
    glEnd();

    glDisable(GL_TEXTURE_2D);
}

void drawRedKey() {
    drawFloorTile();

    GLfloat key_ambient[] = { 0.5f, 0.0f, 0.0f, 1.0f };
    GLfloat key_diffuse[] = { 0.9f, 0.1f, 0.1f, 1.0f };
    GLfloat key_specular[] = { 1.0f, 0.3f, 0.3f, 1.0f };

    glMaterialfv(GL_FRONT, GL_AMBIENT, key_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, key_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, key_specular);
    glMaterialf(GL_FRONT, GL_SHININESS, 50.0f);

    glPushMatrix();
    glTranslatef(0, 0.2f, 0);
    glScalef(0.0002f, 0.0002f, 0.0002f);

    if (g_redKeyTex != 0) {
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, g_redKeyTex);
    }

    if (g_redKeyLoaded) {
        drawOBJ(g_redKeyMesh);
    }
    else {
        glutSolidCube(1.0);
    }

    if (g_redKeyTex != 0) {
        glBindTexture(GL_TEXTURE_2D, 0);
        glDisable(GL_TEXTURE_2D);
    }

    glPopMatrix();
}

void drawBlueKey() {
    drawFloorTile();

    GLfloat key_ambient[] = { 0.0f, 0.0f, 0.5f, 1.0f };
    GLfloat key_diffuse[] = { 0.1f, 0.1f, 0.9f, 1.0f };
    GLfloat key_specular[] = { 0.3f, 0.3f, 1.0f, 1.0f };

    glMaterialfv(GL_FRONT, GL_AMBIENT, key_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, key_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, key_specular);
    glMaterialf(GL_FRONT, GL_SHININESS, 50.0f);

    glPushMatrix();
    glTranslatef(0, 0.2f, 0);
    glScalef(0.0002f, 0.0002f, 0.0002f);

    if (g_blueKeyTex != 0) {
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, g_blueKeyTex);
    }

    if (g_blueKeyLoaded) {
        drawOBJ(g_blueKeyMesh);
    }
    else {
        glutSolidCube(1.0);
    }

    if (g_blueKeyTex != 0) {
        glBindTexture(GL_TEXTURE_2D, 0);
        glDisable(GL_TEXTURE_2D);
    }

    glPopMatrix();
}

void drawYellowKey() {
    drawFloorTile();

    GLfloat key_ambient[] = { 0.5f, 0.5f, 0.0f, 1.0f };
    GLfloat key_diffuse[] = { 0.9f, 0.9f, 0.1f, 1.0f };
    GLfloat key_specular[] = { 1.0f, 1.0f, 0.3f, 1.0f };

    glMaterialfv(GL_FRONT, GL_AMBIENT, key_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, key_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, key_specular);
    glMaterialf(GL_FRONT, GL_SHININESS, 50.0f);

    glPushMatrix();
    glTranslatef(0, 0.2f, 0);
    glScalef(0.0002f, 0.0002f, 0.0002f);

    if (g_yellowKeyTex != 0) {
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, g_yellowKeyTex);
    }

    if (g_yellowKeyLoaded) {
        drawOBJ(g_yellowKeyMesh);
    }
    else {
        glutSolidCube(1.0);
    }

    if (g_yellowKeyTex != 0) {
        glBindTexture(GL_TEXTURE_2D, 0);
        glDisable(GL_TEXTURE_2D);
    }

    glPopMatrix();
}

void drawGoal(bool activated) {
    drawFloorTile();

    GLfloat goal_ambient[4], goal_diffuse[4], goal_specular[4];

    if (activated) {
        goal_ambient[0] = 0.0f; goal_ambient[1] = 0.5f; goal_ambient[2] = 0.0f; goal_ambient[3] = 1.0f;
        goal_diffuse[0] = 0.0f; goal_diffuse[1] = 1.0f; goal_diffuse[2] = 0.0f; goal_diffuse[3] = 1.0f;
        goal_specular[0] = 0.3f; goal_specular[1] = 1.0f; goal_specular[2] = 0.3f; goal_specular[3] = 1.0f;
    }
    else {
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
    int   segments = 20;

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

            case TILE_PANEL_WALL:   // 패널 벽
                drawPanelWall();
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
                drawGoal(player.goalActivated);
                break;
            case TILE_LAMP:
                drawLamp();
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


void drawLamp() {
    drawFloorTile();

    GLfloat stand_ambient[] = { 0.2f, 0.2f, 0.2f, 1.0f };
    GLfloat stand_diffuse[] = { 0.4f, 0.4f, 0.4f, 1.0f };
    GLfloat stand_specular[] = { 0.3f, 0.3f, 0.3f, 1.0f };

    glMaterialfv(GL_FRONT, GL_AMBIENT, stand_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, stand_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, stand_specular);
    glMaterialf(GL_FRONT, GL_SHININESS, 20.0f);

    GLUquadric* quad = gluNewQuadric();

    glPushMatrix();
    glTranslatef(0.0f, 0.0f, 0.0f);
    glRotatef(-90.0f, 1, 0, 0);
    gluCylinder(quad, 0.1, 0.1, 2.0, 12, 1);
    glPopMatrix();

    GLfloat bulb_ambient[] = { 0.8f, 0.7f, 0.4f, 1.0f };
    GLfloat bulb_diffuse[] = { 1.0f, 0.9f, 0.6f, 1.0f };
    GLfloat bulb_specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    GLfloat bulb_emission[] = { 0.9f, 0.8f, 0.5f, 1.0f };

    glMaterialfv(GL_FRONT, GL_AMBIENT, bulb_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, bulb_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, bulb_specular);
    glMaterialfv(GL_FRONT, GL_EMISSION, bulb_emission);
    glMaterialf(GL_FRONT, GL_SHININESS, 80.0f);

    glPushMatrix();
    glTranslatef(0.0f, 2.1f, 0.0f);
    glutSolidSphere(0.3, 16, 16);
    glPopMatrix();

    GLfloat no_emission[] = { 0.0f, 0.0f, 0.0f, 1.0f };
    glMaterialfv(GL_FRONT, GL_EMISSION, no_emission);

    gluDeleteQuadric(quad);
}

void setupLamps()
{
    for (int i = 0; i < 3; ++i) {
        glDisable(GL_LIGHT1 + i);
    }

    int lightIndex = 0;

    for (int i = 0; i < MAZE_ROWS; ++i) {
        for (int j = 0; j < MAZE_COLS; ++j) {
            if (MAZE[i][j] == TILE_LAMP && lightIndex < 3) {
                float x = i * CELL_SIZE - (MAZE_ROWS * CELL_SIZE) / 2.0f;
                float z = j * CELL_SIZE - (MAZE_COLS * CELL_SIZE) / 2.0f;
                float y = 2.0f; 

                GLfloat pos[] = { x, y, z, 1.0f };
                GLfloat diffuse[] = { 1.0f, 0.85f, 0.6f, 1.0f };
                GLfloat ambient[] = { 0.2f, 0.17f, 0.12f, 1.0f };
                GLfloat specular[] = { 1.0f, 0.9f, 0.7f, 1.0f };

                GLenum lightId = GL_LIGHT1 + lightIndex;

                glEnable(lightId);
                glLightfv(lightId, GL_POSITION, pos);
                glLightfv(lightId, GL_DIFFUSE, diffuse);
                glLightfv(lightId, GL_AMBIENT, ambient);
                glLightfv(lightId, GL_SPECULAR, specular);

                glLightf(lightId, GL_CONSTANT_ATTENUATION, 0.5f);
                glLightf(lightId, GL_LINEAR_ATTENUATION, 0.1f);
                glLightf(lightId, GL_QUADRATIC_ATTENUATION, 0.02f);

                ++lightIndex;
            }
        }
    }
}

static GLuint compileShaderFromFile(GLenum type, const char* path)
{
    std::ifstream file(path);
    if (!file.is_open()) {
        std::cerr << "Cannot open shader file: " << path << "\n";
        return 0;
    }

    std::stringstream ss;
    ss << file.rdbuf();
    std::string src = ss.str();
    const char* srcCStr = src.c_str();

    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &srcCStr, nullptr);
    glCompileShader(shader);

    GLint compiled = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
    if (!compiled) {
        GLint len = 0;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &len);
        std::vector<char> log(len);
        glGetShaderInfoLog(shader, len, &len, log.data());
        std::cerr << "Shader compile error in " << path << ":\n"
            << log.data() << "\n";
        glDeleteShader(shader);
        return 0;
    }

    return shader;
}

bool initWallShader()
{
    GLuint vs = compileShaderFromFile(GL_VERTEX_SHADER, "wall_parallax.vert");
    GLuint fs = compileShaderFromFile(GL_FRAGMENT_SHADER, "wall_parallax.frag");

    if (!vs || !fs) {
        std::cerr << "Failed to compile wall shaders.\n";
        return false;
    }

    g_wallShader = glCreateProgram();
    glAttachShader(g_wallShader, vs);
    glAttachShader(g_wallShader, fs);
    glLinkProgram(g_wallShader);

    GLint linked = 0;
    glGetProgramiv(g_wallShader, GL_LINK_STATUS, &linked);
    if (!linked) {
        GLint len = 0;
        glGetProgramiv(g_wallShader, GL_INFO_LOG_LENGTH, &len);
        std::vector<char> log(len);
        glGetProgramInfoLog(g_wallShader, len, &len, log.data());
        std::cerr << "Program link error (wall shader):\n"
            << log.data() << "\n";

        glDeleteProgram(g_wallShader);
        g_wallShader = 0;
        return false;
    }

    glDeleteShader(vs);
    glDeleteShader(fs);

    std::cout << "Wall shader initialized. Program id = "
        << g_wallShader << std::endl;
    return true;
}
