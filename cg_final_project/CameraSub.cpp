#include <iostream>
#include <vector>
#include <gl/glew.h>
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h>
#include <gl/glm/glm.hpp>
#include <gl/glm/ext.hpp>
#include <gl/glm/gtc/matrix_transform.hpp>
#include "MyUtils.h"
#include "MyExtern.h"
#include "MyStruct.h"
#include "CameraSub.h"

CameraSub::CameraSub(glm::vec3 &c, glm::vec3 &v) { // c : center, v : view point
	this->camera_h = 100.0f;
	this->eye = glm::vec3(c.x, this->camera_h, c.z);
	this->at = c;
	this->up = v;
	this->center = glm::vec3(0, 0, 0);
	this->viewPoint = glm::vec3(0, 0, 1);
}

CameraSub::~CameraSub() {

}

void CameraSub::SetCenterViewPoint(glm::vec3 &c, glm::vec3 &v) {
	this->center = c;
	this->viewPoint = v;
}

void CameraSub::ChangeViewMat() const {
	gViewMat = glm::lookAt(this->eye, this->at, this->up);
}

void CameraSub::UpdateVectors() {
	this->eye = glm::vec3(this->center.x, this->camera_h, this->center.z);
	this->at = this->center;
	this->up = this->viewPoint;
}

float CameraSub::GetHeight() const {
	return this->camera_h;
}

glm::vec3 CameraSub::GetEye() const {
	return this->eye;
}