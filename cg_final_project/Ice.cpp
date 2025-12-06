#include <iostream>
#include <vector>
#include <algorithm>
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
#include "Ice.h"
#include "Tank.h"
#include "Boss.h"

Ice::Ice(Model* model, Tank* target, glm::vec3 loc)
	: VAO(0), VBO_pos(0), VBO_nol(0), EBO(0) {
	// 변수 초기화
	this->model = model;
	this->vCount = model->vertex_count, this->fCount = model->face_count;
	this->uColor = glm::vec3(0.0f, 0.0f, 0.0f);
	this->uAlpha = 0.5f;
	this->uLightColorLoc = 0, this->uLightPosLoc = 0, this->uViewPosLoc = 0, this->uObjColorLoc = 0;
	this->uProjLoc = 0, this->uViewLoc = 0, this->uModelLoc = 0;
	this->velocity = 60.0f;
	this->center = loc;
	this->viewPoint = glm::vec3(0, 0, 0);
	this->target = target;
	this->modelMat = glm::translate(glm::mat4(1.0), this->center) * glm::scale(glm::mat4(1.0), glm::vec3(8, 8, 8));
	this->transMat = glm::mat4(1.0);

	this->isDestroyed = false;
	this->boundRadius = 0.05;

	// 노말 데이터가 있는지 확인
	if (model->normals == nullptr) {
		std::cerr << "ERROR: Model normals are not loaded!" << std::endl;
		return;
	}

	InitBuffers(VAO, VBO_pos, VBO_nol, EBO, model->vertices, model->normals,
		model->faces, vCount, fCount, model->normal_count);
	SetColor();
	SetViewPoint();
	SetCenter();
	SetModelMat();

	glUseProgram(shaderProgramID);

	this->uLightPosLoc = glGetUniformLocation(shaderProgramID, "lightPos");
	this->uLightColorLoc = glGetUniformLocation(shaderProgramID, "lightColor");


	this->uModelLoc = glGetUniformLocation(shaderProgramID, "model");
	this->uViewLoc = glGetUniformLocation(shaderProgramID, "view");
	this->uProjLoc = glGetUniformLocation(shaderProgramID, "projection");

	this->uViewPosLoc = glGetUniformLocation(shaderProgramID, "viewPos");
	this->uObjColorLoc = glGetUniformLocation(shaderProgramID, "objectColor");

	this->uAlphaLoc = glGetUniformLocation(shaderProgramID, "alpha");
}

Ice::~Ice() {
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

void Ice::SetColor() {
	this->uColor = glm::vec3(0, 0, 1);
}

void Ice::SetViewPoint() {
	if (this->target != nullptr) {
		// 포탄에서 몬스터를 향하는 벡터를 viewpoint로 업데이트
		glm::vec3 targetCenter = this->target->GetCenter();
		this->viewPoint = glm::normalize(targetCenter - this->center);
	}
	else {
		glm::vec3 targetCenter = myBoss->GetCenter();
		glm::vec3 targetVec = glm::vec3(0, 0, 0);
		targetVec.x = targetCenter.x - this->center.x;
		targetVec.y = targetCenter.y - this->center.y + (myBoss->GetSize() / 2.0f);
		targetVec.z = targetCenter.z - this->center.z;
		this->viewPoint = glm::normalize(targetVec);
	}
}

void Ice::SetModelMat() {
	// 다음 움직임을 적용하는 행렬 결정
	glm::vec3 deltaMove = this->velocity * this->viewPoint * gDeltaTime;
	this->transMat = glm::translate(glm::mat4(1.0), deltaMove);
	this->modelMat = this->transMat * this->modelMat;
}

void Ice::SetCenter() {
	// 다음 움직임을 적용하는 행렬을 현재 위치에 적용 -> center 업데이트
	glm::vec4 vector = glm::vec4(this->center, 1);
	vector = this->transMat * vector;
	this->center = glm::vec3(vector);
}

void Ice::Update() {
	if (CollisionWithTarget()) {
		this->target->Frozen();
		this->isDestroyed = true;
	}
	// 순서가 중요
	SetViewPoint(); // target 의 움직임에 따라 시선을 업데이트
	SetModelMat(); // 업데이트된 시선과 속도에 따라 변환 행렬 업데이트
	SetCenter(); // 업데이트된 행렬에 따라 위치값 업데이트
}

glm::vec3 Ice::GetCenter() {
	return this->center;
}

void Ice::Draw(std::string camera) {
	glUseProgram(shaderProgramID);
	glBindVertexArray(VAO);

	//glUniform3f(uLightPosLoc, 0.0, 200.0, 100.0);
	//glUniform3f(uLightColorLoc, 1.0, 1.0, 1.0);
	glUniform3f(uLightPosLoc, lightPos.x, lightPos.y, lightPos.z);
	glUniform3f(uLightColorLoc, lightColor.x, lightColor.y, lightColor.z);

	glUniformMatrix4fv(uModelLoc, 1, GL_FALSE, glm::value_ptr(modelMat));
	glUniformMatrix4fv(uViewLoc, 1, GL_FALSE, glm::value_ptr(gViewMat));
	glUniformMatrix4fv(uProjLoc, 1, GL_FALSE, glm::value_ptr(gProjMat));

	glm::vec3 cameraPos = glm::vec3(0, 0, 0);
	if (camera == "main")
		cameraPos = myMainCamera->GetEye();

	glUniform3f(uViewPosLoc, cameraPos.x, cameraPos.y, cameraPos.z);
	glUniform3fv(uObjColorLoc, 1, glm::value_ptr(uColor));
	glUniform1f(uAlphaLoc, uAlpha);
	glDrawElements(GL_TRIANGLES, fCount * 3, GL_UNSIGNED_INT, (void*)(0));
	glBindVertexArray(0);
}

bool Ice::GetDestroyed() {
	return this->isDestroyed;
}

bool Ice::CollisionWithTarget() {
	// collision 구현
	float tr = gTankSize_depth / 2.0f; // tr : target bound radius
	glm::vec3 tc = this->target->GetCenter(); // tc : target center
	glm::vec3 c = this->center; // c : this object center
	float distance =
		(c.x - tc.x) * (c.x - tc.x) +
		(c.y - tc.y) * (c.y - tc.y) +
		(c.z - tc.z) * (c.z - tc.z);
	if (distance <= (tr + this->boundRadius) * tr + this->boundRadius) {
		return true;
	}
	return false;
}