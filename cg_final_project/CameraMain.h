#pragma once

#include <iostream>
#include <vector>
#include <gl/glew.h>
#include <gl/freeglut.h>
#include <gl/glm/glm.hpp>

class CameraMain
{
public:
	CameraMain(glm::vec3 &c, glm::vec3 &v); // called from Tank::Tank()
	~CameraMain();
	void SetModelMat(glm::mat4 &m); // called from Tank::Update()
	void ChangeViewMat() const; // called from DrawScene()
	void UpdateVectors(); // called from Timer()
	glm::vec3 GetEye() const;

private:
	glm::vec3 eye;
	glm::vec3 at;
	glm::vec3 up;
	glm::mat4 modelMat;
	float camera_d; // distance
	float camera_h; // height
};

