#version 330 core
layout (location = 0) in vec3 aPos;

out vec3 TexCoords;

uniform mat4 view;
uniform mat4 projection;

void main()
{
    TexCoords = aPos;
    vec4 pos = projection * view * vec4(aPos, 1.0);
    // Z값을 1.0(최대 깊이)으로 고정하여 항상 가장 뒤에 그려지게 함
    gl_Position = pos.xyww; 
}