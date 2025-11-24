#version 330 core
out vec4 FragColor;

in vec3 TexCoords;

uniform samplerCube skybox;
uniform vec3 skyColor; // [추가됨] C++에서 받아올 하늘 색상

void main()
{    
    // 텍스처 대신 우리가 계산한 색상을 바로 출력
    FragColor = vec4(skyColor, 1.0);
}