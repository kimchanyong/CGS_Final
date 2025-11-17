
# CGS_Final – OpenGL 기반 미로 탐험 프로젝트

## 🔧 Project Setup

### 0. 기본세팅은 실습과 동일

### 1. Install FreeGLUT
Download & unzip:
- include → <project>/freeglut-3.2.1_artech/include
- lib → <project>/freeglut-3.2.1_artech/lib/x64
- dll → copy freeglut.dll next to executable

### 2. Visual Studio Settings
- C/C++ → Additional Include Directories  
  `freeglut-3.2.1_artech/include`
- Linker → Additional Library Directories  
  `freeglut-3.2.1_artech/lib/x64`

### 3. Build
- Set configuration to x64
- Build → Run (freeglut.dll must be next to exe)


## 🌿 Branch Strategy

- main: stable version
- 주의사항: 각자 브랜치 생성해서 자기 브랜치에서만 작업하기

<초기세팅>
- git bash 실행
- git clone https://github.com/kimchanyong/CGS_Final.git
- git checkout -b (브랜치이름)
- git push -u origin (브랜치이름)

<작업이 끝나면>
- git add .
- git commit -m "(한줄요약)"
- git push

