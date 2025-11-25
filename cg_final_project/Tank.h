#pragma once

#include <iostream>
#include <vector>
#include <gl/glew.h>
#include <gl/freeglut.h>
#include <gl/glm/glm.hpp>

class TankPart;
struct Model;

class Tank
{
public:
	// model : data(vertex, face, normal)
	Tank(Model* bottomModel, Model* midModel, Model* topModel, Model* barrelModel);
	~Tank();
	// set member from keyboard or mouse
	void SetIsFront(bool value);
	void SetIsBack(bool value);
	void SetIsLeft(bool value);
	void SetIsRight(bool value);
	void SetIsJumping(bool value);
	// update
	void Update();
	void SetTransMat();
	void SetRotateMat();
	void SetRemaining();
	// draw
	void DrawAllPart(std::string str);

private:
	bool isFront, isBack;
	bool isLeft, isRight;
	bool isJumping;

	TankPart* bottom; // (bottom->mid->top->barrel)
	TankPart* mid;
	TankPart* top;
	TankPart* barrel;

	glm::vec3 center; // for updating camera vec
	glm::vec3 frontVec; // for updating camera vec

	glm::mat4 transMat; // press 'w' or 's' (Apply to bottom, mid, top, barrel)
	glm::mat4 rotateMat; // press 'a' or 'd' (Apply to bottom, mid, top, barrel)
	glm::mat4 modelMat; // transMat * rotateMat (final transformation matrix)

	float maxSpeed;
	float acceleration;
	float deceleration;
	float currentSpeed;

	float rVelocity; // rotation velocity
	float rRadians; // rotation radians
};

