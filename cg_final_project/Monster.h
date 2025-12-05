#pragma once

#include <gl/glew.h>
#include <gl/freeglut.h>
#include <gl/glm/glm.hpp>

struct Model;
class Tank;

class Monster
{
public:
    Monster(Model* model, Tank* target, glm::vec3 initLoc, bool isThrown); // 생성될 때 탱크 포인터를 받는다.
    ~Monster();
    void SpeedUp();
    void SetColor();
    void SetViewPoint();
    void SetTransMat();
    void SetRotateMat();
    void SetModelMat();
    void SetCenter();
    void Update();
    glm::vec3 GetCenter();
    void Draw(std::string camera);
    void TakeDamage(float attack);
    void SetDestroyed(bool a);
    bool GetDestroyed();
    bool CollisionWithTarget();
    float GetBoundRadius();
    void ApplyKnockback();
    void ApplyThrowing();
    bool IsAtOrigin();

private:
    Model* model;
    GLuint VAO;
    GLuint VBO_pos, VBO_nol, EBO;
    GLuint vCount, fCount;
    GLuint uModelLoc, uViewLoc, uProjLoc;
    GLuint uLightPosLoc, uLightColorLoc, uObjColorLoc, uViewPosLoc, uAlphaLoc;
    glm::vec3 uColor;
    float uAlpha;
    glm::mat4 modelMat, transMat, rotateMat;
    Tank* target;
    glm::vec3 viewPoint, frontVec, center;
    float rotateSpeed;
    float velocity;
    float yVelocity;
    float jumpForce;
    float gravity;
    float atk;
    float hp;
    bool isDestroyed;
    float boundRadius;
    bool isKnockbacking, isJumping, isOnGround, isThrown;
    
    float maxKnockbackDis;
    float currKnockbackDis;

    float width;
    float depth;

    glm::vec3 thrownTarget;
    float xThrowDis, zThrowDis;
    float xThrowForce, zThrowForce;
};

