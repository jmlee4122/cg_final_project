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
#include "CameraSub.h"
#include "Boss.h"
#include "Tank.h"

Boss::Boss(Model* model, Tank* target, int count) : VAO(0), VBO_pos(0), VBO_nol(0), EBO(0) {
	this->model = model;
	this->vCount = model->vertex_count, this->fCount = model->face_count;
	this->uColor = glm::vec3(0.0f, 0.0f, 0.0f);
	this->uLightColorLoc = 0, this->uLightPosLoc = 0, this->uViewPosLoc = 0, this->uObjColorLoc = 0;
	this->uProjLoc = 0, this->uViewLoc = 0, this->uModelLoc = 0;
	this->velocity = 5.0f;
	this->yVelocity = 0.0f;
	this->jumpForce = 50.0f;
	this->gravity = -1.0f;
	this->center = glm::vec3(0, 0, 0);
	this->viewPoint = glm::vec3(0, 0, 1);
	this->target = target;
	this->size = 20.0f;
	this->atk_basic = 10.0f, this->atk_jump = 40.0f, this->hp = 10 * count;
	this->modelMat = glm::scale(glm::mat4(1.0), glm::vec3(this->size, this->size, this->size));
	this->transMat = glm::mat4(1.0);
	this->isDestroyed = false;

	if (model->normals == nullptr) {
		std::cerr << "ERROR: Model normals are not loaded!" << std::endl;
		return;
	}

	InitBuffers(VAO, VBO_pos, VBO_nol, EBO, model->vertices, model->normals,
		model->faces, vCount, fCount, model->normal_count);
	SetColor();
	SetViewPoint();
	SetModelMat();
	SetCenter();

	glUseProgram(shaderProgramID);

	this->uLightPosLoc = glGetUniformLocation(shaderProgramID, "lightPos");
	this->uLightColorLoc = glGetUniformLocation(shaderProgramID, "lightColor");


	this->uModelLoc = glGetUniformLocation(shaderProgramID, "model");
	this->uViewLoc = glGetUniformLocation(shaderProgramID, "view");
	this->uProjLoc = glGetUniformLocation(shaderProgramID, "projection");

	this->uViewPosLoc = glGetUniformLocation(shaderProgramID, "viewPos");
	this->uObjColorLoc = glGetUniformLocation(shaderProgramID, "objectColor");
}

Boss::~Boss() {
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO_pos);
	glDeleteBuffers(1, &VBO_nol);
	glDeleteBuffers(1, &EBO);
	if (this->model != nullptr) {
		if (this->model->vertices) free(this->model->vertices);
		if (this->model->normals) free(this->model->normals);
		if (this->model->faces) free(this->model->faces);
		delete this->model;
		this->model = nullptr;
	}
}

void Boss::SetColor() {
	this->uColor = glm::vec3(1.0f / this->hp, 0.0f, 1.0f / this->hp);
}

void Boss::SetViewPoint() {
	// 몬스터에서 탱크를 향하는 벡터를 viewpoint로 업데이트
	glm::vec3 targetCenter = this->target->GetCenter();
	this->viewPoint = glm::normalize(targetCenter - this->center);
}

void Boss::SetTransMat() {
	glm::vec3 transVec = glm::vec3(0, 0, 0);
	glm::vec3 jumpVec = glm::vec3(0, 0, 0);
	transVec = this->velocity * this->viewPoint * gDeltaTime;
	transVec.y = 0.0f;
}

void Boss::SetModelMat() {

}

void Boss::SetCenter() {

}

void Boss::Update() {

}