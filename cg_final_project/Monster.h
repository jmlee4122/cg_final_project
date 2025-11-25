#pragma once

#include <gl/glew.h>
#include <gl/freeglut.h>
#include <gl/glm/glm.hpp>

struct Model;
class Tank;

class Monster
{
public:
    Monster(Model* model, Tank* target); // 생성될 때 탱크 포인터를 받는다.
    ~Monster();
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
    Tank* target;
    glm::vec3 viewPoint;
    glm::vec3 center;
    float velocity;
};

