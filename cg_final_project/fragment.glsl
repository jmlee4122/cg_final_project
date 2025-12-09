#version 330 core

in vec3 FragPos;
in vec3 Normal;
out vec4 FragColor;

uniform vec3 lightColor;
uniform vec3 lightPos;
uniform vec3 viewPos;
uniform vec3 objectColor;
uniform float alpha;

void main()
{
    vec3 ambientLight = vec3(0.3);
    vec3 ambient = ambientLight * lightColor;

    vec3 normalVector = normalize(Normal);
    
    // 양면 조명: 노말이 뒤집혀도 조명 적용
    if (dot(normalVector, normalize(viewPos - FragPos)) < 0.0) {
        normalVector = -normalVector;  // 노말 반전
    }
    
    vec3 lightDir = normalize(lightPos - FragPos);

    float diffuseLight = max(dot(normalVector, lightDir), 0.0);
    vec3 diffuse = diffuseLight * lightColor;

    int shininess = 32;
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, normalVector);

    float specularLight = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
    vec3 specular = specularLight * lightColor * 0.5;

    vec3 result = (ambient + diffuse + specular) * objectColor;
    FragColor = vec4(result, alpha);
}