#include <GL/glew.h> 
#include <GL/glut.h>

#include <iostream>
#include <string>
#include <cmath>

#include <SDL.h>
#include <SDL_mixer.h>

#include "maze.h"
#include "model.h"
#include "player.h"
#include "collision.h"
#include "tagger.h"

Mix_Music* gBgm = nullptr;
bool gAudioInitialized = false;

int lastTime = 0;
int windowWidth = 1280;
int windowHeight = 720;

void initModels();
bool initWallShader();   


void drawCenteredText(const std::string& text, float y, void* font = GLUT_BITMAP_HELVETICA_18) {
    int textWidth = 0;
    for (char c : text) { 
        textWidth += glutBitmapWidth(font, c);
    }

    float x = (windowWidth - textWidth) / 2.0f;

    glRasterPos2f(x, y);

    for (char c : text) {
        glutBitmapCharacter(font, c);
    }
}

void initOpenGL() {
    glClearColor(0.1f, 0.1f, 0.15f, 1.0f);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);

    GLfloat light_position[] = { 0.0f, 20.0f, 0.0f, 1.0f };
    GLfloat light_ambient[] = { 0.4f, 0.4f, 0.4f, 1.0f };
    GLfloat light_diffuse[] = { 0.9f, 0.9f, 0.9f, 1.0f };
    GLfloat light_specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };

    glLightfv(GL_LIGHT0, GL_POSITION, light_position);
    glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);

    glShadeModel(GL_SMOOTH);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // 텍스처 사용
    glEnable(GL_TEXTURE_2D);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

    glutSetCursor(GLUT_CURSOR_NONE);

    std::cout << "OpenGL initialized successfully" << std::endl;
}

bool initAudio()
{
    if (SDL_Init(SDL_INIT_AUDIO) < 0) {
        std::cout << "SDL init error: " << SDL_GetError() << std::endl;
        return false;
    }

    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        std::cout << "Mix_OpenAudio error: " << Mix_GetError() << std::endl;
        return false;
    }

    // 🔊 BGM 파일 로드 (프로젝트 폴더에 bgm.ogg 넣어놨다고 가정)
    gBgm = Mix_LoadMUS("bgm.wav");   // .wav / .mp3도 가능
    if (!gBgm) {
        std::cout << "Failed to load bgm: " << Mix_GetError() << std::endl;
        return false;
    }

    // 🔁 -1 = 무한 반복
    if (Mix_PlayMusic(gBgm, -1) == -1) {
        std::cout << "Failed to play bgm: " << Mix_GetError() << std::endl;
        return false;
    }

    gAudioInitialized = true;
    std::cout << "Audio initialized & BGM playing.\n";
    return true;
}

void shutdownAudio()
{
    if (!gAudioInitialized) return;

    Mix_HaltMusic();

    if (gBgm) {
        Mix_FreeMusic(gBgm);
        gBgm = nullptr;
    }

    Mix_CloseAudio();
    SDL_Quit();   // 오디오만 썼으니 전체 Quit해도 됨
    gAudioInitialized = false;
}

void renderGameOverScreen() {
    glClearColor(0, 0, 0, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, windowWidth, 0, windowHeight);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    if (player.isCaught) {
        drawCenteredText("GAME OVER", windowHeight * 0.55f, GLUT_BITMAP_TIMES_ROMAN_24);
        drawCenteredText("You were caught by the Tagger", windowHeight * 0.45f);
    }
    else if (player.reachedGoal) {
        drawCenteredText("CONGRATULATIONS!", windowHeight * 0.55f, GLUT_BITMAP_TIMES_ROMAN_24);
        drawCenteredText("YOU ESCAPED!", windowHeight * 0.45f);
    }

    drawCenteredText("Press ESC to exit", windowHeight * 0.30f);

    glutSwapBuffers();
}

// =============================
// 미니맵 렌더링
// =============================
void renderMiniMap()
{
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, windowWidth, 0, windowHeight);  // 화면 픽셀 좌표

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);
    glDisable(GL_TEXTURE_2D);

    float mapSize = 200.0f;
    float margin = 20.0f;
    float originX = windowWidth - mapSize - margin;
    float originY = windowHeight - mapSize - margin;

    float tileW = mapSize / (float)MAZE_COLS;
    float tileH = mapSize / (float)MAZE_ROWS;

    // 배경 박스
    glColor4f(0.0f, 0.0f, 0.0f, 0.6f);
    glBegin(GL_QUADS);
    glVertex2f(originX - 5, originY - 5);
    glVertex2f(originX + mapSize + 5, originY - 5);
    glVertex2f(originX + mapSize + 5, originY + mapSize + 5);
    glVertex2f(originX - 5, originY + mapSize + 5);
    glEnd();

    // 타일들
    for (int i = 0; i < MAZE_ROWS; ++i) {
        for (int j = 0; j < MAZE_COLS; ++j) {

            float x = originX + j * tileW;
            float y = originY + (MAZE_ROWS - 1 - i) * tileH;  // 위에서 아래로

            int tile = MAZE[i][j];

            switch (tile) {
            case TILE_WALL:
                glColor3f(0.2f, 0.2f, 0.2f);
                break;
            case TILE_START:
                glColor3f(0.0f, 0.6f, 0.0f);
                break;
            case TILE_GOAL:
                glColor3f(1.0f, 1.0f, 0.0f);
                break;
            case TILE_RED_KEY:
                glColor3f(1.0f, 0.0f, 0.0f);
                break;
            case TILE_BLUE_KEY:
                glColor3f(0.0f, 0.4f, 1.0f);
                break;
            case TILE_YELLOW_KEY:
                glColor3f(1.0f, 1.0f, 0.0f);
                break;
            case TILE_LAMP:
                glColor3f(1.0f, 0.7f, 0.2f);
                break;
            case TILE_PANEL_WALL:   // 패널 벽이면 색 다르게 주고 싶으면 여기
                glColor3f(0.7f, 0.7f, 0.7f);
                break;
            default:
                glColor3f(0.05f, 0.05f, 0.08f);
                break;
            }

            glBegin(GL_QUADS);
            glVertex2f(x, y);
            glVertex2f(x + tileW, y);
            glVertex2f(x + tileW, y + tileH);
            glVertex2f(x, y + tileH);
            glEnd();
        }
    }

    // 플레이어 위치 표시
    float offsetX = (MAZE_ROWS * CELL_SIZE) / 2.0f;
    float offsetZ = (MAZE_COLS * CELL_SIZE) / 2.0f;

    int pi = (int)((player.x + offsetX) / CELL_SIZE);
    int pj = (int)((player.z + offsetZ) / CELL_SIZE);

    if (pi >= 0 && pi < MAZE_ROWS && pj >= 0 && pj < MAZE_COLS) {
        float px = originX + pj * tileW + tileW * 0.5f;
        float py = originY + (MAZE_ROWS - 1 - pi) * tileH + tileH * 0.5f;

        glColor3f(0.0f, 1.0f, 0.0f); // 플레이어
        float r = std::min(tileW, tileH) * 0.3f;

        glBegin(GL_TRIANGLE_FAN);
        glVertex2f(px, py);
        for (int k = 0; k <= 16; ++k) {
            float a = 2.0f * 3.1415926f * k / 16.0f;
            glVertex2f(px + cos(a) * r, py + sin(a) * r);
        }
        glEnd();
    }

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glEnable(GL_TEXTURE_2D);

    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
}

// =============================
// 3D 렌더링
// =============================
void render() {

    if (player.isCaught || player.reachedGoal) {
        renderGameOverScreen();
        return;
    }

    glClearColor(0.1f, 0.1f, 0.15f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    setupCamera();
    setupLamps();

    drawMaze();
    drawTagger();

    renderMiniMap();

    glutSwapBuffers();
}

// =============================
// 매 프레임 업데이트
// =============================
void update() {
    int currentTime = glutGet(GLUT_ELAPSED_TIME);
    float deltaTime = (currentTime - lastTime) / 1000.0f;
    lastTime = currentTime;

    updatePlayer(deltaTime);
    updateTagger(deltaTime);

    glutPostRedisplay();
}

// =============================
// 창 크기 변경
// =============================
void reshape(int width, int height) {
    if (height == 0) height = 1;

    windowWidth = width;
    windowHeight = height;

    glViewport(0, 0, width, height);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(70.0, (float)width / (float)height, 0.1, 200.0);

    glMatrixMode(GL_MODELVIEW);
}

// =============================
// 콘솔 설명 출력
// =============================
void printInstructions() {
    std::cout << "\n============================================" << std::endl;
    std::cout << "  OpenGL Maze Game - Tag Game (술래잡기)" << std::endl;
    std::cout << "============================================" << std::endl;
    std::cout << "\nControls:" << std::endl;
    std::cout << "  W/A/S/D or Arrow Keys - Move" << std::endl;
    std::cout << "  MOUSE - Look around" << std::endl;
    std::cout << "  ESC - Exit" << std::endl;
    std::cout << "\nObjective:" << std::endl;
    std::cout << "  1. Collect all 3 keys" << std::endl;
    std::cout << "  2. Reach the EXIT!" << std::endl;
    std::cout << "\nWARNING:" << std::endl;
    std::cout << "  The RED TAGGER is hunting you!" << std::endl;
    std::cout << "============================================\n" << std::endl;
}

// =============================
// main 함수
// =============================
int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
    glutInitWindowSize(windowWidth, windowHeight);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("OpenGL Maze - Tag Game");

    GLenum err = glewInit();
    if (err != GLEW_OK) {
        std::cerr << "GLEW init error: "
            << glewGetErrorString(err) << std::endl;
        return 1;
    }
    std::cout << "GLEW initialized. OpenGL version: "
        << glGetString(GL_VERSION) << std::endl;


    initOpenGL();
    initModels();          // 🔹 OBJ 모델들 로드

    if (!loadMaze("maze.txt")) {
        std::cerr << "Failed to load maze!" << std::endl;
        return 1;
    }

    if (!initMazeTextures()) {
        std::cerr << "Failed to init maze textures!" << std::endl;
    }

    // 🔹 여기서 벽 패럴랙스/노멀맵 셰이더 초기화
    if (!initWallShader()) {
        std::cerr << "Failed to init wall shader!" << std::endl;
        // 셰이더 실패해도 기본 벽 렌더링만 쓰고 계속 진행해도 됨
    }

    buildCollisionMap();
    initPlayer();
    initTagger();

    if (!initAudio()) {
        std::cerr << "Audio init failed. Game will run without sound.\n";
    }

    glutDisplayFunc(render);
    glutIdleFunc(update);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(onKeyPress);
    glutKeyboardUpFunc(onKeyRelease);
    glutSpecialFunc(onSpecialKeyPress);
    glutSpecialUpFunc(onSpecialKeyRelease);
    glutPassiveMotionFunc(onMouseMove);

    printInstructions();

    lastTime = glutGet(GLUT_ELAPSED_TIME);
    glutWarpPointer(windowWidth / 2, windowHeight / 2);

    glutMainLoop();
    return 0;
}
