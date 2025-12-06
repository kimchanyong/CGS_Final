#ifndef TAGGER_H
#define TAGGER_H

#include "util.h"
#include <vector>

// A* 알고리즘용 노드 구조체
struct PathNode {
    int x, z;              // 미로 타일 좌표
    float g;               // 시작점으로부터의 실제 비용 (G cost)
    float h;               // 목표까지의 추정 비용 (H cost - 휴리스틱)
    float f;               // 총 비용 (F = G + H)
    PathNode* parent;      // 경로 역추적용 부모 노드

    PathNode() : x(0), z(0), g(0), h(0), f(0), parent(nullptr) {}
    PathNode(int x, int z) : x(x), z(z), g(0), h(0), f(0), parent(nullptr) {}
};

// 술래 구조체
struct Tagger {
    // 위치 정보
    float x, y, z;         // 월드 좌표
    int tileX, tileZ;      // 현재 타일 좌표 (A* 경로 추적용)

    // 이동 정보
    float speed;           // 이동 속도 (플레이어보다 약간 느림)
    float targetX, targetZ; // 다음 목표 위치 (월드 좌표)

    // 경로 정보
    std::vector<PathNode*> path;  // A*로 계산된 경로
    int currentPathIndex;          // 현재 경로상의 위치

    // 시간 정보
    float pathUpdateTimer;         // 경로 재계산 타이머
    float pathUpdateInterval;      // 경로 재계산 주기 (초)

    // 상태 정보
    bool isActive;                 // 술래 활성화 여부
    float detectionRadius;         // 플레이어 감지 범위
    float catchRadius;             // 플레이어 잡기 범위

    float facingAngle;             // 🔹 현재 바라보는 Y축 각도(도 단위)

    Tagger() : x(0), y(0), z(0), tileX(0), tileZ(0),
        speed(0.06f),  // 플레이어(0.08)보다 느림
        targetX(0), targetZ(0),
        currentPathIndex(0),
        pathUpdateTimer(0), pathUpdateInterval(0.1f),  
        isActive(true),
        detectionRadius(30.0f),  
        catchRadius(0.8f),
        facingAngle(0.0f)        
    {
    }
};

// 전역 술래 객체
extern Tagger tagger;

// 술래 관련 함수
void initTagger();                                    // 술래 초기화 (0번 타일에서만 스폰)
void updateTagger(float deltaTime);                   // 술래 업데이트
void drawTagger();                                    // 술래 렌더링
bool checkTaggerCatch(float playerX, float playerZ);  // 플레이어 잡기 체크

// A* 알고리즘 함수
std::vector<PathNode*> findPath(int startX, int startZ, int goalX, int goalZ);  // A* 경로 탐색
float heuristic(int x1, int z1, int x2, int z2);     // 휴리스틱 함수 (맨해튼 거리)
void clearPath(std::vector<PathNode*>& path);         // 경로 메모리 해제

#endif