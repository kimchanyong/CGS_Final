#include <GL/glut.h>
#include <iostream>
#include "maze.h"
#include "player.h"
#include "collision.h"

int lastTime = 0;
int windowWidth = 1280;
int windowHeight = 720;

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

    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);

    glShadeModel(GL_SMOOTH);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // 마우스 커서 숨기기
    glutSetCursor(GLUT_CURSOR_NONE);

    std::cout << "OpenGL initialized successfully" << std::endl;
}

void render() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    setupCamera();
    drawMaze();

    glutSwapBuffers();
}

void update() {
    int currentTime = glutGet(GLUT_ELAPSED_TIME);
    float deltaTime = (currentTime - lastTime) / 1000.0f;
    lastTime = currentTime;

    updatePlayer(deltaTime);

    // 마우스를 화면 중앙으로 재배치
    glutWarpPointer(windowWidth / 2, windowHeight / 2);
    lastMouseX = windowWidth / 2;
    lastMouseY = windowHeight / 2;

    glutPostRedisplay();
}

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

void printInstructions() {
    std::cout << "\n====================================" << std::endl;
    std::cout << "  OpenGL Maze Game" << std::endl;
    std::cout << "====================================" << std::endl;
    std::cout << "\nControls:" << std::endl;
    std::cout << "  W/A/S/D or Arrow Keys - Move" << std::endl;
    std::cout << "  MOUSE - Look around" << std::endl;
    std::cout << "  ESC - Exit" << std::endl;
    std::cout << "\nObjective:" << std::endl;
    std::cout << "  Collect all items (golden cubes)" << std::endl;
    std::cout << "  Reach the goal (green cylinder)" << std::endl;
    std::cout << "\nMaze Tile Types:" << std::endl;
    std::cout << "  -1 = Start position" << std::endl;
    std::cout << "   0 = Empty space" << std::endl;
    std::cout << "   1 = Wall" << std::endl;
    std::cout << "   2 = Item" << std::endl;
    std::cout << "   3 = Goal" << std::endl;
    std::cout << "===================================\n" << std::endl;
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
    glutInitWindowSize(windowWidth, windowHeight);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("OpenGL Maze Project - Final");

    initOpenGL();

    if (!loadMaze("maze.txt")) {
        std::cout << "Using default maze" << std::endl;
    }

    buildCollisionMap();

    initPlayer();
    player.totalItems = countTotalItems();
    std::cout << "Objective: Collect all " << player.totalItems << " items!" << std::endl;

    // 콜백 등록
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

    // 마우스를 화면 중앙으로 초기화
    glutWarpPointer(windowWidth / 2, windowHeight / 2);

    glutMainLoop();

    return 0;
}