#pragma once

#include <gl/glew.h>
#include <gl/freeglut.h>
#include <gl/glm/glm.hpp>

struct Model;
class Tank;

class Ice
{
public:
    Ice(Model* model, Tank* target, glm::vec3 loc); // 포탄은 몬스터를 포인터로 받음
    ~Ice();
    void SetColor();
    void SetViewPoint();
    void SetModelMat();
    void SetCenter();
    void Update();
    glm::vec3 GetCenter();
    void Draw(std::string camera);
    bool GetDestroyed();
    bool CollisionWithTarget();

private:
    Model* model;
    GLuint VAO;
    GLuint VBO_pos, VBO_nol, EBO;
    GLuint vCount, fCount;
    GLuint uModelLoc, uViewLoc, uProjLoc;
    GLuint uLightPosLoc, uLightColorLoc, uObjColorLoc, uViewPosLoc, uAlphaLoc;
    glm::vec3 uColor;
    float uAlpha;
    glm::mat4 modelMat;
    glm::mat4 transMat;
    Tank* target;
    glm::vec3 viewPoint;
    glm::vec3 center;
    float velocity;
    bool isDestroyed;
    float boundRadius;
};

