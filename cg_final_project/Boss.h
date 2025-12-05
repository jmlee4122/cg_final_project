#pragma once

#include <gl/glew.h>
#include <gl/freeglut.h>
#include <gl/glm/glm.hpp>

struct Model;
class Tank;

class Boss
{
public:
    Boss(Model* model, Tank* target, glm::vec3 initLoc); // 생성될 때 탱크 포인터를 받는다.
    ~Boss();
    void SetColor();
    void SetViewPoint();
    void SetScaleMat();
    void SetTransMat();
    void SetModelMat();
    void SetRotateMat();
    void SetCenter();
    void Update();
    glm::vec3 GetCenter();
    void Draw(std::string camera);
    void TakeDamage(float attack);
    bool GetDestroyed();
    bool CollisionWithTarget();
    void ApplyKnockback();
    float GetBoundRadius();
    void IncreaseSize(int cnt);
    glm::mat4 GetModelMat();
    float GetSize();
    void ThrowAttack();

private:
    Model* model;
    GLuint VAO;
    GLuint VBO_pos, VBO_nol, EBO;
    GLuint vCount, fCount;
    GLuint uModelLoc, uViewLoc, uProjLoc;
    GLuint uLightPosLoc, uLightColorLoc, uObjColorLoc, uViewPosLoc, uAlphaLoc;
    glm::vec3 uColor;
    float uAlpha;
    glm::mat4 scaleMat;
    glm::mat4 modelMat;
    glm::mat4 transMat;
    glm::mat4 rotateMat;
    Tank* target;
    glm::vec3 viewPoint;
    glm::vec3 frontVec;
    glm::vec3 center;
    float rotateSpeed;
    float velocity, yVelocity;
    float jumpForce, gravity;
    float atk_basic, atk_jump, hp, maxHp;
    float size;
    float boundRadius;
    bool isDestroyed;
    bool isJumping;
    bool isOnGround;
    bool isKnockbacking;
    float maxKnockbackDis;
    float currKnockbackDis;
    float lastThrowAttackTime;
    float throwCoolTime;
};

