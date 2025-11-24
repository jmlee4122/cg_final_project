#version 330 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

uniform sampler2D texture1;
uniform vec3 lightPos;
uniform vec3 lightColor;
// viewPos는 지형에서 스펙큘러(반짝임)를 줄일거라 굳이 안써도 되지만, 확장성을 위해 남김

void main()
{
    // 1. Ambient (주변광)
    float ambientStrength = 0.4;
    vec3 ambient = ambientStrength * lightColor;
  
    // 2. Diffuse (확산광)
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;
    
    // 지형(잔디)은 반짝임(Specular)이 적으므로 생략하거나 아주 약하게 처리
    
    // 텍스처 색상 가져오기
    vec4 texColor = texture(texture1, TexCoords);
    
    // 최종 색상 = (주변광 + 확산광) * 텍스처색
    vec3 result = (ambient + diffuse) * texColor.rgb;
    FragColor = vec4(result, 1.0);
}