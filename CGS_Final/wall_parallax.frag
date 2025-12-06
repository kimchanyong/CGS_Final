// wall_parallax.frag
#version 120

varying vec2 vTexCoord;
varying vec3 vFragPosEye;
varying mat3 vTBN;

uniform sampler2D uDiffuseMap; // 컬러
uniform sampler2D uNormalMap;  // 노멀
uniform sampler2D uHeightMap;  // 높이 (패럴랙스)

vec2 parallaxMapping(vec2 texCoords, vec3 viewDirTangent)
{
    float height = texture2D(uHeightMap, texCoords).r;
    float heightScale = 0.04; // 튀어나오는 정도 (0.02~0.08 사이에서 조절해봐)
    return texCoords + viewDirTangent.xy * (height * heightScale);
}

void main()
{
    // 카메라는 eye space에서 (0,0,0)에 있다고 가정
    vec3 viewDirEye = normalize(-vFragPosEye);

    // tangent 공간으로 변환
    vec3 viewDirT = normalize(vTBN * viewDirEye);

    // 간단히 "손전등"처럼: 빛 방향 = 카메라 방향
    vec3 lightDirT = viewDirT;

    // 1) 패럴랙스 매핑으로 텍스처 좌표 보정
    vec2 texCoords = parallaxMapping(vTexCoord, viewDirT);

    // 범위 벗어나면 버리기 (벽 바깥)
    if (texCoords.x < 0.0 || texCoords.x > 1.0 ||
        texCoords.y < 0.0 || texCoords.y > 1.0) {
        discard;
    }

    // 2) 노멀맵 (0~1 → -1~1)
    vec3 normalSample = texture2D(uNormalMap, texCoords).rgb;
    vec3 N = normalize(normalSample * 2.0 - 1.0);

    // 3) 디퓨즈(컬러) 텍스처
    vec3 albedo = texture2D(uDiffuseMap, texCoords).rgb;

    // Blinn-Phong 조명 (lightDirT / viewDirT / N 사용)
    vec3 L = normalize(lightDirT);
    vec3 V = normalize(viewDirT);
    vec3 H = normalize(L + V);

    float diff = max(dot(N, L), 0.0);
    float spec = pow(max(dot(N, H), 0.0), 32.0);

    vec3 ambient  = 0.2 * albedo;
    vec3 diffuse  = diff * albedo;
    vec3 specular = 0.4 * spec * vec3(1.0);

    vec3 color = ambient + diffuse + specular;
    gl_FragColor = vec4(color, 1.0);
}
