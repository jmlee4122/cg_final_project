#include <iostream>
#include <vector>
#include <gl/glew.h>
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h>
#include <gl/glm/glm.hpp>
#include <gl/glm/ext.hpp>
#include <gl/glm/gtc/matrix_transform.hpp>
#include "Tank.h"
#include "TankPart.h"
#include "CameraMain.h"
#include "CameraSub.h"
#include "MyUtils.h"
#include "MyExtern.h"
#include "MyStruct.h"
#include "Monster.h"
#include "Bullet.h"

Tank::Tank(Model* bottomModel, Model* midModel, Model* topModel, Model* barrelModel) {
    // create tank part
    this->bottom = new TankPart(bottomModel, "bottom");
    this->mid = new TankPart(midModel, "mid");
    this->top = new TankPart(topModel, "top");
    this->barrel = new TankPart(barrelModel, "barrel");

    // connect tank part
    this->bottom->SetChild(this->mid); // bottom -> mid
    this->mid->SetChild(this->top); // mid -> top
    this->top->SetChild(this->barrel); // top -> barrel

    this->isFront = false, this->isBack = false;
    this->isLeft = false, this->isRight = false;
    this->isJumping = false, this->isOnGround = true;

    this->center = glm::vec3(0, 0, 0);
    this->frontVec = glm::vec3(0, 0, 1);
    this->transMat = glm::mat4(1.0);
    this->rotateMat = glm::mat4(1.0);
    this->modelMat = glm::mat4(1.0);
    this->viewPoint = this->frontVec;
    this->viewRotSpeed = 8.0f;
    this->viewRotateMat = glm::mat4(1.0);

    // init speed and acceleration (translation)
    this->maxSpeed = 0.5f;
    this->acceleration = 1.0f; // per frame
    this->deceleration = 1.0f; // per frame
    this->currentSpeed = 0.0f;

    // init speed and radians (rotation)
    this->rRadians = 0.15f;
    this->rVelocity = this->currentSpeed / this->rRadians;

    this->yVelocity = 0.0f;
    this->jumpForce = 20.0f;
    this->gravity = -1.0f;

    this->barrelLen = 1.3f;

    this->nearest = nullptr;
    this->atk = 10.0f;
    this->hp = 100.0f;
    this->isDestroyed = false;

    this->width = gTankSize_width;
    this->depth = gTankSize_depth;

    // setting camera (myExtern)
    myMainCamera = new CameraMain(this->center, this->frontVec);
    mySubCamera = new CameraSub(this->center, this->frontVec);
}
Tank::~Tank() {
    if (this->bottom) {
        delete this->bottom;
        this->bottom = nullptr;
    }
    if (this->mid) {
        delete this->mid;
        this->mid = nullptr;
    }
    if (this->top) {
        delete this->top;
        this->top = nullptr;
    }
    if (this->barrel) {
        delete this->barrel;
        this->barrel = nullptr;
    }
}

void Tank::SetIsFront(bool value) { this->isFront = value; }
void Tank::SetIsBack(bool value) { this->isBack = value; }
void Tank::SetIsLeft(bool value) { this->isLeft = value; }
void Tank::SetIsRight(bool value) { this->isRight = value; }
void Tank::SetIsJumping(bool value) { this->isJumping = value; }

void Tank::Update() {
    // find & update nearest monster
    this->nearest = NearestMonster();

    // update matrix
    SetTransMat();
    SetRotateMat();
    SetViewRotMat();
    SetModelMat();
    
    this->top->SetModelMat(this->viewRotateMat);
    
    // from bottom to barrel (recursion)
    glm::mat4 transformMat = this->transMat * this->rotateMat;
    this->bottom->SetModelMat(transformMat);

    // update camera
    SetRemaining();
    //UpdateCameraVectors(this->modelMat, this->center, this->frontVec); // in MyUtils.h
    myMainCamera->SetModelMat(transformMat);
    mySubCamera->SetCenterViewPoint(this->center, this->frontVec);
}
void Tank::SetTransMat() {
    float targetSpeed = 0.0f;
    glm::vec3 transVec = glm::vec3(0, 0, 0);
    glm::vec3 jumpVec = glm::vec3(0, 0, 0);

    // go forward => target speed : plus max speed
    if (isFront && !isBack) targetSpeed = this->maxSpeed;
    // go backward => target speed : minus max speed
    else if (!isFront && isBack) targetSpeed = -1.0f * this->maxSpeed;
    // standing => target speed : 0
    else targetSpeed = 0.0f;

    // acceleration or deceleration
    if (this->currentSpeed < targetSpeed) {
        // acceleration
        this->currentSpeed += this->acceleration * gDeltaTime;
        if (targetSpeed < this->currentSpeed) {
            this->currentSpeed = targetSpeed;
        }
    }
    else if (targetSpeed < this->currentSpeed) {
        // deceleration
        this->currentSpeed -= (targetSpeed == 0.0f) ? this->deceleration * gDeltaTime : this->acceleration * gDeltaTime;
        if (this->currentSpeed < targetSpeed) {
            this->currentSpeed = targetSpeed;
        }
    }

    // current speed : nearly 0 => 0 (standing)
    if (glm::abs(this->currentSpeed) < 0.001f) {
        this->currentSpeed = 0.0f;
        transVec = glm::vec3(0, 0, 0);
    }
    else {
        glm::vec3 frontVector = glm::normalize(this->frontVec);
        transVec = this->currentSpeed * frontVector;

        float currentFootY = this->center.y;
        float nextX = this->center.x + transVec.x;
        if (CheckCollision(nextX, this->center.z, currentFootY, this->width, this->depth)) {
            transVec.x = 0.0f; // 충돌 있으면 이동 적용 x
        }
        float nextZ = this->center.z + transVec.z;
        if (CheckCollision(this->center.x, nextZ, currentFootY, this->width, this->depth)) {
            transVec.z = 0.0f; // 충돌 있으면 이동 적용 x
        }
    }

    if (isJumping) {
        if (isOnGround) {
            this->isOnGround = false;
            this->yVelocity = this->jumpForce;
        }
    }
    this->yVelocity += this->gravity;
    float nextY = this->center.y + (this->yVelocity * gDeltaTime);
    if (nextY <= GetTerrainHeight(this->center.x, this->center.z)) {
        nextY = GetTerrainHeight(this->center.x, this->center.z);
        this->isOnGround = true;
        this->isJumping = false;
        this->yVelocity = 0.0f;
    }
    float deltaY = nextY - this->center.y;
    jumpVec = glm::vec3(0, deltaY, 0);
    glm::vec3 move = transVec + jumpVec;
    this->transMat = glm::translate(glm::mat4(1.0), move);
}

void Tank::SetRotateMat() {
    // isLeft == isRight => not rotating
    if (this->isLeft == this->isRight) {
        this->rotateMat = glm::mat4(1.0);
    }
    // rotating
    else {
        if (this->isLeft) {
            this->rVelocity = this->currentSpeed / this->rRadians;
        }
        if (this->isRight) {
            this->rVelocity = -1.0f * this->currentSpeed / this->rRadians;
        }
        glm::mat4 t1 = glm::translate(glm::mat4(1.0), -1.0f * this->center);
        glm::mat4 r = glm::rotate(glm::mat4(1.0), (float)glm::radians(this->rVelocity), glm::vec3(0, 1, 0));
        glm::mat4 t2 = glm::translate(glm::mat4(1.0), this->center);
        this->rotateMat = t2 * r * t1;
    }
}
void Tank::SetViewRotMat() {
    glm::vec3 targetVec = glm::vec3(0, 0, 0);

    if (this->nearest == nullptr) {
        targetVec = this->frontVec;
    }
    else {
        glm::vec3 nc = this->nearest->GetCenter();

        targetVec.x = nc.x - this->center.x;
        targetVec.y = 0.0f;
        targetVec.z = nc.z - this->center.z;
    }
    float len = 
        (targetVec.x * targetVec.x) +
        (targetVec.y * targetVec.y) +
        (targetVec.z * targetVec.z);
    if (len < 0.001f) {
        this->viewRotateMat = glm::mat4(1.0);
        return;
    }

    glm::vec3 normViewPoint = glm::normalize(this->viewPoint);
    glm::vec3 normTargetVec = glm::normalize(targetVec);
    float cosTheta = glm::dot(normViewPoint, normTargetVec);
    float degree = acos(glm::clamp(cosTheta, -1.0f, 1.0f));
    // (+) : left side, (-) : right side
    float dirSign = glm::cross(normViewPoint, normTargetVec).y;
    float viewRotRadian = 0.0f;
    if (dirSign > 0.0f) {
        viewRotRadian = degree * this->viewRotSpeed;
    }
    else if (dirSign < 0.0f) {
        viewRotRadian = -degree * this->viewRotSpeed;
    }
    else {
        viewRotRadian = 0.0f;
    }

    glm::mat4 t1 = glm::translate(glm::mat4(1.0), -this->center);
    glm::mat4 r = glm::rotate(
        glm::mat4(1.0), glm::radians(viewRotRadian), glm::vec3(0, 1, 0));
    glm::mat4 t2 = glm::translate(glm::mat4(1.0), this->center);

    this->viewRotateMat = t2 * r * t1;
}
void Tank::SetModelMat() {
    this->modelMat = this->transMat * this->rotateMat * this->modelMat;
}

void Tank::SetRemaining() {
    // updating center
    glm::vec4 vector = glm::vec4(this->center, 1); // order : 3 -> 4
    vector = this->transMat * vector;
    this->center = glm::vec3(vector); // order : 4 -> 3

    // updating frontVec
    vector = glm::vec4(this->frontVec, 0); // order : 3 -> 4
    vector = this->rotateMat * vector;
    this->frontVec = glm::normalize(glm::vec3(vector)); // order : 4 -> 3

    // updating view point
    vector = glm::vec4(this->viewPoint, 0);
    vector = this->rotateMat * this->viewRotateMat * vector;
    this->viewPoint = glm::normalize(glm::vec3(vector));
}

void Tank::DrawAllPart(std::string str) {
    // from bottom to barrel (recursion)
    this->bottom->DrawPart(str);
}

glm::vec3 Tank::GetCenter() {
    return this->center;
}
Monster* Tank::GetNearest() {
    return nearest;
}
glm::mat4 Tank::GetModelMat() {
    return this->modelMat;
}

void Tank::attack() {
    if (this->nearest == nullptr) return;
    // GetBulletInitLoc() : 현재 위치에서 포탄이 생성될 위치 (포구 바로 앞)
    glm::vec3 location = GetBulletInitLoc();
    Model* bulletModel = new Model;
    read_obj_file("bullet.obj", bulletModel);
    Bullet* newBullet = new Bullet(bulletModel, this->nearest, location, this->atk);
    myBullets.push_back(newBullet);
}

Monster* Tank::NearestMonster() {
    Monster* nearest = nullptr;
    float minDis = (float)1e9;
    for (int i = 0; i < myMonsters.size(); i++) {
        // tc: tank center, mc: monster center
        glm::vec3 tc = this->center;
        glm::vec3 mc = myMonsters[i]->GetCenter();
        // 크기만 비교하므로 루트를 씌우지 x
        float currDis =
            (tc.x - mc.x) * (tc.x - mc.x) +
            (tc.y - mc.y) * (tc.y - mc.y) +
            (tc.z - mc.z) * (tc.z - mc.z);
        if (currDis < minDis) {
            minDis = currDis;
            nearest = myMonsters[i];
        }
    }
    // 가장 가까운 몬스터 반환, 몬스터가 없다면 null 반환
    return nearest;
}

glm::vec3 Tank::GetBulletInitLoc() {
    glm::vec3 loc = glm::vec3(0, 0, 0);
    loc = this->center + (this->barrelLen * this->viewPoint);
    return loc;
}

void Tank::TakeDamage(float attack) {
    if (this->hp - attack <= 0.0f) {
        this->hp = 0.0f;
        this->isDestroyed = true;
    }
    else {
        this->hp -= attack;
    }
    std::cout << this->hp << " ";
}