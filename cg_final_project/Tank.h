#pragma once

#include <iostream>
#include <vector>
#include <gl/glew.h>
#include <gl/freeglut.h>
#include <gl/glm/glm.hpp>

class TankPart;
struct Model;
class Monster;

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
	void SetViewRotMat();
	void SetModelMat();
	void SetRemaining();
	// draw
	void DrawAllPart(std::string str);
	// pass center to Monsters
	glm::vec3 GetCenter();
	// pass neartest monster to bulletW
	Monster* GetNearest();
	glm::mat4 GetModelMat();

	void attack();
	Monster* NearestMonster();
	glm::vec3 GetBulletInitLoc();
	void TakeDamage(float attack);

	void Respawn();

private:
	bool isFront, isBack;
	bool isLeft, isRight;
	bool isJumping, isOnGround;

	TankPart* bottom; // (bottom->mid->top->barrel)
	TankPart* mid;
	TankPart* top;
	TankPart* barrel;

	glm::vec3 center; // for updating camera vec
	glm::vec3 frontVec; // for updating camera vec
	glm::vec3 viewPoint;

	glm::mat4 transMat; // press 'w' or 's' (Apply to bottom, mid, top, barrel)
	glm::mat4 rotateMat; // press 'a' or 'd' (Apply to bottom, mid, top, barrel)
	glm::mat4 modelMat; // transMat * rotateMat (final transformation matrix)
	
	float viewRotSpeed;
	glm::mat4 viewRotateMat;

	float maxSpeed;
	float acceleration;
	float deceleration;
	float currentSpeed;

	float rVelocity; // rotation velocity
	float rRadians; // rotation radians

	float yVelocity;
	float jumpForce;
	float gravity;

	float barrelLen;

	Monster* nearest;

	float atk;
	float hp;
	bool isDestroyed;

	float width;
	float depth;
};

