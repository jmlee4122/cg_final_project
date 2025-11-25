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
#include "CameraMain.h"

CameraMain::CameraMain(glm::vec3 &c, glm::vec3 &v) { // c : center, v : view point
	this->camera_d = gTankSize_depth * 10.0f;
	this->camera_h = gTankSize_height * 25.0f;

	this->at = c;
	this->eye.x = this->at.x + camera_d * cos(glm::radians(pitch)) * sin(glm::radians(yaw));
	this->eye.y = this->at.y + camera_h * sin(glm::radians(pitch));
	this->eye.z = this->at.z + camera_d * cos(glm::radians(pitch)) * cos(glm::radians(yaw));
	this->up = glm::vec3(0, 1, 0);
	this->modelMat = glm::mat4(1.0);
}

CameraMain::~CameraMain() {

}

void CameraMain::SetModelMat(glm::mat4 &m) {
	this->modelMat = m;
}

void CameraMain::ChangeViewMat() const {
	gViewMat = glm::lookAt(this->eye, this->at, this->up);
}

void CameraMain::UpdateVectors() {
	// update at vector
	glm::vec4 vector = glm::vec4(this->at, 1); // order : 3 -> 4
	vector = this->modelMat * vector;
	this->at = glm::vec3(vector); // order : 4 -> 3

	// update eye vector
	this->eye.x = this->at.x + camera_d * cos(glm::radians(pitch)) * sin(glm::radians(yaw));
	this->eye.y = this->at.y + camera_h * sin(glm::radians(pitch));
	this->eye.z = this->at.z + camera_d * cos(glm::radians(pitch)) * cos(glm::radians(yaw));
}

glm::vec3 CameraMain::GetEye() const {
	return this->eye;
}