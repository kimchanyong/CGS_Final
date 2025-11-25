#include "tagger.h"
#include "maze.h"
#include "collision.h"
#include "player.h"
#include <GL/glut.h>
#include <cmath>
#include <algorithm>
#include <queue>
#include <limits>
#include <iostream>

// 전역 술래 객체
Tagger tagger;

// =============================
// 좌표 변환 도우미
// =============================
static void worldToTile(float worldX, float worldZ, int& tileX, int& tileZ) {
    tileX = (int)((worldX + (MAZE_ROWS * CELL_SIZE) / 2.0f) / CELL_SIZE);
    tileZ = (int)((worldZ + (MAZE_COLS * CELL_SIZE) / 2.0f) / CELL_SIZE);

    if (tileX < 0) tileX = 0;
    if (tileX >= MAZE_ROWS) tileX = MAZE_ROWS - 1;
    if (tileZ < 0) tileZ = 0;
    if (tileZ >= MAZE_COLS) tileZ = MAZE_COLS - 1;
}

static void tileToWorld(int tileX, int tileZ, float& worldX, float& worldZ) {
    worldX = tileX * CELL_SIZE - (MAZE_ROWS * CELL_SIZE) / 2.0f;
    worldZ = tileZ * CELL_SIZE - (MAZE_COLS * CELL_SIZE) / 2.0f;
}

// =============================
// 휴리스틱 (맨해튼 거리)
// =============================
float heuristic(int x1, int z1, int x2, int z2) {
    return static_cast<float>(std::abs(x1 - x2) + std::abs(z1 - z2));
}

// =============================
// 리팩토링된 A* findPath
// =============================
std::vector<PathNode*> findPath(int startX, int startZ, int goalX, int goalZ) {
    std::vector<PathNode*> path;

    if (startX < 0 || startX >= MAZE_ROWS ||
        startZ < 0 || startZ >= MAZE_COLS ||
        goalX < 0 || goalX >= MAZE_ROWS ||
        goalZ < 0 || goalZ >= MAZE_COLS) {
        return path;
    }

    if (MAZE[startX][startZ] == TILE_WALL || MAZE[goalX][goalZ] == TILE_WALL) {
        return path;
    }

    const float INF = std::numeric_limits<float>::infinity();

    // 타일 정보 구조체
    struct CellInfo {
        float g;
        float h;
        bool inOpen;
        bool inClosed;
        int parentX;
        int parentZ;
    };

    CellInfo info[MAX_MAZE_SIZE][MAX_MAZE_SIZE];

    for (int i = 0; i < MAZE_ROWS; ++i) {
        for (int j = 0; j < MAZE_COLS; ++j) {
            info[i][j].g = INF;
            info[i][j].h = 0;
            info[i][j].inOpen = false;
            info[i][j].inClosed = false;
            info[i][j].parentX = -1;
            info[i][j].parentZ = -1;
        }
    }

    struct PQNode {
        int x, z;
        float f;
    };

    auto cmp = [](const PQNode& a, const PQNode& b) {
        return a.f > b.f;
        };

    std::priority_queue<PQNode, std::vector<PQNode>, decltype(cmp)> openList(cmp);

    info[startX][startZ].g = 0.0f;
    info[startX][startZ].h = heuristic(startX, startZ, goalX, goalZ);
    openList.push({ startX, startZ, info[startX][startZ].g + info[startX][startZ].h });
    info[startX][startZ].inOpen = true;

    const int dx[4] = { -1, 1, 0, 0 };
    const int dz[4] = { 0, 0, -1, 1 };

    bool found = false;

    while (!openList.empty()) {
        PQNode cur = openList.top();
        openList.pop();

        int cx = cur.x;
        int cz = cur.z;

        if (info[cx][cz].inClosed) continue;
        info[cx][cz].inClosed = true;

        if (cx == goalX && cz == goalZ) {
            found = true;
            break;
        }

        for (int i = 0; i < 4; i++) {
            int nx = cx + dx[i];
            int nz = cz + dz[i];

            if (nx < 0 || nx >= MAZE_ROWS || nz < 0 || nz >= MAZE_COLS)
                continue;

            if (MAZE[nx][nz] == TILE_WALL)
                continue;

            if (info[nx][nz].inClosed)
                continue;

            float newG = info[cx][cz].g + 1.0f;

            if (newG < info[nx][nz].g) {
                info[nx][nz].g = newG;
                info[nx][nz].h = heuristic(nx, nz, goalX, goalZ);
                info[nx][nz].parentX = cx;
                info[nx][nz].parentZ = cz;

                float f = info[nx][nz].g + info[nx][nz].h;
                openList.push({ nx, nz, f });
                info[nx][nz].inOpen = true;
            }
        }
    }

    if (!found) return path;

    int x = goalX;
    int z = goalZ;

    while (x != -1 && z != -1) {
        PathNode* node = new PathNode(x, z);
        node->g = info[x][z].g;
        node->h = info[x][z].h;
        node->f = node->g + node->h;
        path.push_back(node);

        int px = info[x][z].parentX;
        int pz = info[x][z].parentZ;
        x = px;
        z = pz;
    }

    std::reverse(path.begin(), path.end());
    return path;
}

// =============================
// 메모리 해제
// =============================
void clearPath(std::vector<PathNode*>& path) {
    for (PathNode* n : path) delete n;
    path.clear();
}

// =============================
// 술래 초기화
// =============================
void initTagger() {
    bool found = false;

    // 맨 아래쪽 / 오른쪽 영역부터 탐색
    for (int i = MAZE_ROWS - 2; i >= 1 && !found; i--) {
        for (int j = MAZE_COLS - 2; j >= 1 && !found; j--) {
            if (MAZE[i][j] == TILE_EMPTY) {
                tagger.tileX = i;
                tagger.tileZ = j;
                tileToWorld(i, j, tagger.x, tagger.z);
                found = true;
            }
        }
    }

    if (!found) {
        for (int i = 1; i < MAZE_ROWS - 1 && !found; i++) {
            for (int j = 1; j < MAZE_COLS - 1 && !found; j++) {
                if (MAZE[i][j] == TILE_EMPTY) {
                    tagger.tileX = i;
                    tagger.tileZ = j;
                    tileToWorld(i, j, tagger.x, tagger.z);
                    found = true;
                }
            }
        }
    }

    if (!found) {
        tagger.x = 0;
        tagger.z = 0;
        tagger.tileX = MAZE_ROWS / 2;
        tagger.tileZ = MAZE_COLS / 2;
    }

    tagger.y = PLAYER_HEIGHT / 2.0f;
    tagger.targetX = tagger.x;
    tagger.targetZ = tagger.z;
    tagger.currentPathIndex = 0;
    tagger.pathUpdateTimer = 0;
    tagger.isActive = true;
}

// =============================
// 술래 업데이트 (개선본)
// =============================
void updateTagger(float deltaTime) {
    if (!tagger.isActive) return;

    // ⛔ 게임오버 또는 클리어 상태에서는 술래도 정지
    if (player.isCaught || player.reachedGoal) return;

    int playerTileX, playerTileZ;
    worldToTile(player.x, player.z, playerTileX, playerTileZ);
    worldToTile(tagger.x, tagger.z, tagger.tileX, tagger.tileZ);

    tagger.pathUpdateTimer += deltaTime;
    bool needRepath = false;

    if (tagger.path.empty()) needRepath = true;
    else if (tagger.pathUpdateTimer >= tagger.pathUpdateInterval) needRepath = true;
    else if (tagger.currentPathIndex >= (int)tagger.path.size()) needRepath = true;

    if (needRepath) {
        tagger.pathUpdateTimer = 0.0f;

        clearPath(tagger.path);
        tagger.path = findPath(tagger.tileX, tagger.tileZ, playerTileX, playerTileZ);
        tagger.currentPathIndex = 0;

        if (!tagger.path.empty()) {
            if (tagger.path.size() > 1) {
                tagger.currentPathIndex = 1;
                PathNode* nextNode = tagger.path[1];
                tileToWorld(nextNode->x, nextNode->z, tagger.targetX, tagger.targetZ);
            }
            else {
                tagger.targetX = player.x;
                tagger.targetZ = player.z;
            }
        }
        else {
            tagger.targetX = player.x;
            tagger.targetZ = player.z;
        }
    }

    float dx = tagger.targetX - tagger.x;
    float dz = tagger.targetZ - tagger.z;
    float dist = std::sqrt(dx * dx + dz * dz);

    if (dist > 0.0001f) {
        dx /= dist;
        dz /= dist;

        float step = tagger.speed;
        float newX = tagger.x + dx * step;
        float newZ = tagger.z + dz * step;

        if (!checkCollision(newX, newZ, PLAYER_RADIUS)) {
            tagger.x = newX;
            tagger.z = newZ;
        }
        else {
            bool moved = false;

            float slideX = tagger.x + dx * step;
            if (!checkCollision(slideX, tagger.z, PLAYER_RADIUS)) {
                tagger.x = slideX;
                moved = true;
            }

            float slideZ = tagger.z + dz * step;
            if (!checkCollision(tagger.x, slideZ, PLAYER_RADIUS)) {
                tagger.z = slideZ;
                moved = true;
            }

            if (!moved) {
                tagger.pathUpdateTimer = tagger.pathUpdateInterval;
            }
        }

        if (!tagger.path.empty() && tagger.currentPathIndex < (int)tagger.path.size()) {
            float tx, tz;
            PathNode* node = tagger.path[tagger.currentPathIndex];
            tileToWorld(node->x, node->z, tx, tz);

            float ddx = tx - tagger.x;
            float ddz = tz - tagger.z;
            float d = std::sqrt(ddx * ddx + ddz * ddz);

            if (d < 0.1f) {
                tagger.currentPathIndex++;
                if (tagger.currentPathIndex < (int)tagger.path.size()) {
                    PathNode* nextNode = tagger.path[tagger.currentPathIndex];
                    tileToWorld(nextNode->x, nextNode->z, tagger.targetX, tagger.targetZ);
                }
                else {
                    tagger.targetX = player.x;
                    tagger.targetZ = player.z;
                }
            }
        }
    }
}

// =============================
// 술래 렌더링 (임시 모델)
// =============================
void drawTagger() {
    if (!tagger.isActive) return;

    glPushMatrix();
    glTranslatef(tagger.x, tagger.y, tagger.z);

    // =============================
    //  ⭐ 여기서부터가 "술래 모델링 교체 지점"
    // =============================
    //
    // 1. 나중에 FBX / OBJ 모델을 사용한다면,
    //    glRotatef(taggerFacingAngle, 0,1,0);  // 이동 방향을 바라보게
    //    glScalef(...);  // 모델 크기 조절
    //    drawYourTaggerModelMesh();  // 파일로부터 로드한 3D 모델 렌더
    //
    // 2. 아래 임시 구체는 나중에 완전히 삭제하고 모델 렌더 함수로 교체 가능
    // =============================

    // --- 몸체 (임시 구체) ---
    GLfloat amb[] = { 0.5f, 0.0f, 0.0f, 1.0f };
    GLfloat diff[] = { 1.0f, 0.0f, 0.0f, 1.0f };
    GLfloat spec[] = { 1.0f, 0.2f, 0.2f, 1.0f };
    glMaterialfv(GL_FRONT, GL_AMBIENT, amb);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, diff);
    glMaterialfv(GL_FRONT, GL_SPECULAR, spec);
    glMaterialf(GL_FRONT, GL_SHININESS, 50.0f);

    glutSolidSphere(PLAYER_RADIUS, 16, 16);

    // --- 눈 (임시) ---
    GLfloat white[] = { 1,1,1,1 };
    glMaterialfv(GL_FRONT, GL_AMBIENT, white);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, white);

    glPushMatrix();
    glTranslatef(-0.15f, 0.1f, 0.3f);
    glutSolidSphere(0.08f, 8, 8);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0.15f, 0.1f, 0.3f);
    glutSolidSphere(0.08f, 8, 8);
    glPopMatrix();

    // =============================
    //  ⭐ 여기까지가 모델 교체 블록
    // =============================

    glPopMatrix();
}

// =============================
// 플레이어 잡기 판정
// =============================
bool checkTaggerCatch(float playerX, float playerZ) {
    if (!tagger.isActive) return false;

    float dx = tagger.x - playerX;
    float dz = tagger.z - playerZ;
    float d = std::sqrt(dx * dx + dz * dz);

    return d < tagger.catchRadius;
}
