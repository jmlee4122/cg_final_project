#pragma once

#include <gl/glew.h>
#include <gl/freeglut.h>
#include <gl/glm/glm.hpp>

struct Model;
class Tank;

class Boss
{
public:
    Boss(Model* model, Tank* target, int count); // 생성될 때 탱크 포인터를 받는다.
    ~Boss();
    void SetColor();
    void SetViewPoint();
    void SetTransMat();
    void SetModelMat();
    void SetCenter();
    void Update();
    glm::vec3 GetCenter();
    void Draw(std::string camera);
    void TakeDamage(float attack);
    bool GetDestroyed();
    bool CollisionWithTarget();
    float GetBoundRadius();
    void ApplyKnockback();

private:
    Model* model;
    GLuint VAO;
    GLuint VBO_pos, VBO_nol, EBO;
    GLuint vCount, fCount;
    GLuint uModelLoc, uViewLoc, uProjLoc;
    GLuint uLightPosLoc, uLightColorLoc, uObjColorLoc, uViewPosLoc;
    glm::vec3 uColor;
    glm::mat4 modelMat;
    glm::mat4 transMat;
    Tank* target;
    glm::vec3 viewPoint;
    glm::vec3 center;
    float velocity, yVelocity;
    float jumpForce, gravity;
    float atk_basic, atk_jump, hp;
    float size;
    bool isDestroyed;
};

