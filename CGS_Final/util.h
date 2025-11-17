#ifndef UTIL_H
#define UTIL_H

#include <cmath>

// 상수 정의
const int MAX_MAZE_SIZE = 30;
const float CELL_SIZE = 2.0f;        // 타일 크기 증가 (1.1 → 2.0)
const float WALL_HEIGHT = 2.5f;      // 벽 높이
const float PLAYER_RADIUS = 0.4f;    // 플레이어 반경
const float PLAYER_HEIGHT = 1.8f;    // 플레이어 키
const float MOVE_SPEED = 0.08f;      // 이동 속도
const float MOUSE_SENSITIVITY = 0.25f;  // 마우스 민감도

// 수학 유틸리티
inline float clamp(float value, float min, float max) {
    if (value < min) return min;
    if (value > max) return max;
    return value;
}

inline void normalize2D(float& x, float& z) {
    float length = std::sqrt(x * x + z * z);
    if (length > 0.001f) {
        x /= length;
        z /= length;
    }
}

#endif