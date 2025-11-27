#pragma once

#include <gl/glew.h>
#include <gl/freeglut.h>
#include <gl/glm/glm.hpp>

struct Model;
class Monster;

class Bullet
{
public:
    Bullet(Model* model, Monster* target, glm::vec3 loc, float attack); // 포탄은 몬스터를 포인터로 받음
    ~Bullet();
    void SetColor();
    void SetViewPoint();
    void SetModelMat();
    void SetCenter();
    void Update();
    glm::vec3 GetCenter();
    void Draw(std::string camera);

private:
    GLuint VAO;
    GLuint VBO_pos, VBO_nol, EBO;
    GLuint vCount, fCount;
    GLuint uModelLoc, uViewLoc, uProjLoc;
    GLuint uLightPosLoc, uLightColorLoc, uObjColorLoc, uViewPosLoc;
    glm::vec3 uColor;
    glm::mat4 modelMat;
    glm::mat4 transMat;
    Monster* target;
    glm::vec3 viewPoint;
    glm::vec3 center;
    float velocity;
    float atk;
};

