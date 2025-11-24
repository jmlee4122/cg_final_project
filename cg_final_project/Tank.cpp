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
    this->isJumping = false;

    this->center = glm::vec3(0, 0, 0);
    this->viewPoint = glm::vec3(0, 0, 1);
    this->transMat = glm::mat4(1.0);
    this->rotateMat = glm::mat4(1.0);
    this->modelMat = glm::mat4(1.0);

    // init speed and acceleration (translation)
    this->maxSpeed = 1.0f;
    this->acceleration = 0.02f; // per frame
    this->deceleration = 0.02f; // per frame
    this->currentSpeed = 0.0f;

    // init speed and radians (rotation)
    this->rRadians = 1.0f;
    this->rVelocity = this->maxSpeed / this->rRadians;

    // setting camera (myExtern)
    myMainCamera = new CameraMain(this->center, this->viewPoint);
    mySubCamera = new CameraSub(this->center, this->viewPoint);
}
Tank::~Tank() {
    // later
}

void Tank::SetIsFront(bool value) { this->isFront = value; }
void Tank::SetIsBack(bool value) { this->isBack = value; }
void Tank::SetIsLeft(bool value) { this->isLeft = value; }
void Tank::SetIsRight(bool value) { this->isRight = value; }
void Tank::SetIsJumping(bool value) { this->isJumping = value; }

void Tank::Update() {
    // update matrix
    SetTransMat();
    SetRotateMat();
    // update modelMat (final transformation matrix)
    this->modelMat = this->transMat * this->rotateMat;

    // from bottom to barrel (recursion)
    this->bottom->SetModelMat(this->modelMat);

    // update camera
    SetRemaining();
    //UpdateCameraVectors(this->modelMat, this->center, this->viewPoint); // in MyUtils.h
    myMainCamera->SetModelMat(this->modelMat);
    mySubCamera->SetCenterViewPoint(this->center, this->viewPoint);
}
void Tank::SetTransMat() {
    float targetSpeed = 0.0f;

    // go forward => target speed : plus max speed
    if (isFront && !isBack) {
        targetSpeed = this->maxSpeed;
    }
    // go backward => target speed : minus max speed
    else if (!isFront && isBack) {
        targetSpeed = -1.0f * this->maxSpeed;
    }
    // standing => target speed : 0
    else {
        targetSpeed = 0.0f;
    }

    // acceleration or deceleration
    if (this->currentSpeed < targetSpeed) {
        // acceleration
        this->currentSpeed += this->acceleration;
        if (targetSpeed < this->currentSpeed) {
            this->currentSpeed = targetSpeed;
        }
    }
    else if (targetSpeed < this->currentSpeed) {
        // deceleration
        this->currentSpeed -= (targetSpeed == 0.0f) ? this->deceleration : this->acceleration;
        if (this->currentSpeed < targetSpeed) {
            this->currentSpeed = targetSpeed;
        }
    }

    // current speed : nearly 0 => 0 (standing)
    if (glm::abs(this->currentSpeed) < 0.001f) {
        this->currentSpeed = 0.0f;
        this->transMat = glm::mat4(1.0);
    }
    else {
        glm::vec3 frontVector = glm::normalize(this->viewPoint);
        glm::vec3 move = this->currentSpeed * frontVector;
        this->transMat = glm::translate(glm::mat4(1.0), move);
    }
}
void Tank::SetRotateMat() {
    // isFront == isBack => not ratating
    // isLeft == isRight => not rotating
    if (this->isLeft == this->isRight) {
        this->rotateMat = glm::mat4(1.0);
    }
    // rotating
    else {
        float w = 0.0f;
        if ((this->isLeft && this->isFront) || (this->isRight && this->isBack)) {
            w = this->rVelocity; // ccw
        }
        if (this->isRight && this->isFront || (this->isLeft && this->isBack)) {
            w = -1.0f * this->rVelocity; // cw
        }
        glm::mat4 t1 = glm::translate(glm::mat4(1.0), -1.0f * this->center);
        glm::mat4 r = glm::rotate(glm::mat4(1.0), (float)glm::radians(w), glm::vec3(0, 1, 0));
        glm::mat4 t2 = glm::translate(glm::mat4(1.0), this->center);
        this->rotateMat = t2 * r * t1;
    }
}
void Tank::SetRemaining() {
    // updating center
    glm::vec4 vector = glm::vec4(this->center, 1); // order : 3 -> 4
    vector = this->transMat * vector;
    this->center = glm::vec3(vector); // order : 4 -> 3

    // updating viewPoint
    vector = glm::vec4(this->viewPoint, 0); // order : 3 -> 4
    vector = this->rotateMat * vector;
    this->viewPoint = glm::normalize(glm::vec3(vector)); // order : 4 -> 3
}

void Tank::DrawAllPart(std::string str) {
    // from bottom to barrel (recursion)
    this->bottom->DrawPart(str);
}