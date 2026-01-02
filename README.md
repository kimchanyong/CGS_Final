```
# 🧩 OpenGL Maze Tag Game (CGS_Final)

OpenGL과 C++을 기반으로 제작한 **미로 탈출 & 술래잡기(Tag Game)** 게임입니다.  
플레이어는 미로를 탐색하며 **3개의 열쇠를 수집**하고,  
미로 안을 배회하는 **술래(Tagger)**를 피해 **출구로 탈출**해야 합니다.

본 프로젝트는 컴퓨터 그래픽스 수업의 기말 과제로 제작되었습니다.

---

## 🎮 게임 개요

- **장르**: 1인칭 3D 미로 탈출 / 술래잡기
- **플랫폼**: Windows (x64)
- **개발 언어**: C++
- **그래픽스**: OpenGL (GLEW, FreeGLUT)
- **사운드**: SDL2, SDL2_mixer

---

## 🕹️ 게임 목표

1. 미로 안에 숨겨진 **3개의 열쇠 수집**
2. 술래(Tagger)에게 잡히지 않고 **출구 도달**
3. 술래에게 잡히면 **Game Over**

---

## 🎯 조작 방법

| 입력 | 동작 |
|----|----|
| W / A / S / D | 이동 |
| 방향키 | 이동 |
| 마우스 | 시점 회전 |
| ESC | 게임 종료 |

---

## 🧠 주요 구현 요소

- 랜덤 미로 생성 시스템
- OBJ 모델 로딩 (열쇠, 술래 캐릭터 등)
- 텍스처 매핑 및 셰이더(Vertex / Fragment Shader)
- 충돌 판정(Collision Map) 기반 이동 제어
- 술래(Tagger)의 추적 로직
- 배경음(BGM) 및 효과음 재생
- 메뉴 / 게임 오버 UI 텍스처 처리

---

## 📁 프로젝트 구조

```

Program/
├─ Source/
│   ├─ main.cpp
│   ├─ maze.cpp / maze.h
│   ├─ player.cpp / player.h
│   ├─ tagger.cpp / tagger.h
│   ├─ collision.cpp
│   ├─ wall_parallax.vert
│   ├─ wall_parallax.frag
│   └─ 기타 cpp / h 파일
│
└─ Bin/
├─ CGS_Final.exe
├─ freeglut.dll
├─ glew32.dll
├─ SDL2.dll
├─ SDL2_mixer.dll
├─ models/
├─ textures/
├─ maze.txt
├─ bgm.wav
└─ key_sound.wav

```

> ⚠️ **주의**  
> 실행 시 `CGS_Final.exe`는 반드시 `Bin` 폴더 안에서 실행해야 합니다.

---

## ▶ 실행 방법

1. `Program.zip` 압축 해제
2. `Program/Bin` 폴더로 이동
3. `CGS_Final.exe` 실행

※ 실행 파일과 DLL, 리소스 파일은 **모두 동일한 폴더 구조**를 유지해야 합니다.

---

## 📌 참고 사항

- 본 저장소에는 **Visual Studio 프로젝트 전체(Debug, x64, .vs 등)**는 포함되어 있지 않습니다.
- 실행에 필요한 최소한의 소스 코드(Source)와 실행 파일(Bin)만 포함되어 있습니다.
- 모든 파일 경로는 **상대 경로**를 기준으로 동작합니다.

---

## 👤 제작자

- **이름**: 김찬용, 이화, 황하린  
- **소속**: 중앙대학교  
- **과목**: 컴퓨터 그래픽스

---

## 📝 라이선스

본 프로젝트는 **학습 및 과제 제출 목적**으로 제작되었습니다.  
상업적 사용은 제한됩니다.
```
