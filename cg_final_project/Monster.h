#pragma once

#include <gl/glew.h>
#include <gl/freeglut.h>
#include <gl/glm/glm.hpp>

struct Model;
class Tank;

class Monster
{
public:
    Monster(Model* model, Tank* target, glm::vec3 initLoc); // 생성될 때 탱크 포인터를 받는다.
    ~Monster();
    void SetColor();
    void SetViewPoint();
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
    float velocity;
    float atk;
    float hp;
    bool isDestroyed;
    float boundRadius;
    bool isKnockbacking;
    float maxKnockbackDis;
    float currKnockbackDis;
};

