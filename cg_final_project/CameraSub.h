#pragma once

#include <iostream>
#include <vector>
#include <gl/glew.h>
#include <gl/freeglut.h>
#include <gl/glm/glm.hpp>

class CameraSub
{
public:
	CameraSub(glm::vec3 &c, glm::vec3 &v); // called from Tank::Tank()
	~CameraSub();
	void SetCenterViewPoint(glm::vec3 &c, glm::vec3 &v); // called from Tank::Update()
	void ChangeViewMat() const; // called from DrawScene()
	void UpdateVectors(); // called from Timer()
	float GetHeight() const;
	glm::vec3 GetEye() const;

private:
	glm::vec3 eye;
	glm::vec3 at;
	glm::vec3 up;
	glm::vec3 center;
	glm::vec3 viewPoint;
	float camera_h;
};