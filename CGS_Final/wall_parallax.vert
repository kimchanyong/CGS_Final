// wall_parallax.vert
#version 120

varying vec2 vTexCoord;
varying vec3 vFragPosEye;
varying mat3 vTBN;

void main()
{
    // 기존 고정 파이프라인 변환 그대로 사용
    gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;

    // 기본 텍스처 좌표
    vTexCoord = gl_MultiTexCoord0.st;

    // 눈(카메라) 좌표계에서의 위치
    vec3 posEye = vec3(gl_ModelViewMatrix * gl_Vertex);
    vFragPosEye = posEye;

    // 눈 좌표계에서의 법선
    vec3 N = normalize(gl_NormalMatrix * gl_Normal);

    // TBN 행렬 만들기 (어떤 방향의 면이든 동작하도록 worldUp 기반)
    vec3 up = vec3(0.0, 1.0, 0.0);
    if (abs(dot(N, up)) > 0.9) {
        // 거의 위를 보고 있으면 다른 up 축 사용
        up = vec3(0.0, 0.0, 1.0);
    }

    vec3 T = normalize(cross(up, N));
    vec3 B = cross(N, T);

    vTBN = mat3(T, B, N);
}
