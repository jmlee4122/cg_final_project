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
#include "Monster.h"
#include "Tank.h"

Monster::Monster(Model* model, Tank* target, glm::vec3 initLoc) : VAO(0), VBO_pos(0), VBO_nol(0), EBO(0) {
	// 변수 초기화
	this->model = model;
	this->vCount = model->vertex_count, this->fCount = model->face_count;
	this->uColor = glm::vec3(0.0f, 0.0f, 0.0f);
	this->uLightColorLoc = 0, this->uLightPosLoc = 0, this->uViewPosLoc = 0, this->uObjColorLoc = 0;
	this->uProjLoc = 0, this->uViewLoc = 0, this->uModelLoc = 0;
	this->velocity = 10.0f;
	this->center = initLoc;
	this->viewPoint = glm::vec3(0, 0, 1);
	this->target = target;
	this->modelMat = glm::translate(glm::mat4(1.0), this->center);
	this->transMat = glm::mat4(1.0);

	this->atk = 5.0f;
	this->hp = 20.0f;
	this->isDestroyed = false;
	this->boundRadius = 0.5f;
	this->isKnockbacking = false;
	this->maxKnockbackDis = 20.0f;
	this->currKnockbackDis = 0.0f;

	// 노말 데이터가 있는지 확인
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

Monster::~Monster() {
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

void Monster::SetColor() {
	if (this->hp > 10.0f) {
		this->uColor = glm::vec3(0.0, 1.0, 0.0);
	}
	else {
		this->uColor = glm::vec3(1.0, 1.0, 0.0);
	}
}

void Monster::SetViewPoint() {
	// 몬스터에서 탱크를 향하는 벡터를 viewpoint로 업데이트
	glm::vec3 targetCenter = this->target->GetCenter();
	this->viewPoint = glm::normalize(targetCenter - this->center);
}

void Monster::SetModelMat() {
	// 다음 움직임을 적용하는 행렬 결정
	glm::vec3 deltaMove = this->velocity * this->viewPoint * gDeltaTime;
	this->transMat = glm::translate(glm::mat4(1.0), deltaMove);
	this->modelMat = this->transMat * this->modelMat;
}

void Monster::SetCenter() {
	// 다음 움직임을 적용하는 행렬을 현재 위치에 적용 -> center 업데이트
	glm::vec4 vector = glm::vec4(this->center, 1);
	vector = this->transMat * vector;
	this->center = glm::vec3(vector);
}

void Monster::Update() {
	if (CollisionWithTarget()) {
		// 자신의 공격력으로 target 에게 피해를 입힘
		this->target->TakeDamage(this->atk);
		this->isKnockbacking = true;
	}
	if (this->isKnockbacking) {
		ApplyKnockback();
	}
	else {
		// 순서가 중요
		SetViewPoint(); // target 의 움직임에 따라 시선을 업데이트
		SetModelMat(); // 업데이트된 시선과 속도에 따라 변환 행렬 업데이트
		SetCenter(); // 업데이트된 행렬에 따라 위치값 업데이트
		SetColor();
	}
}

glm::vec3 Monster::GetCenter() {
	return this->center;
}

void Monster::Draw(std::string camera) {
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
	else if (camera == "sub")
		cameraPos = mySubCamera->GetEye();

	glUniform3f(uViewPosLoc, cameraPos.x, cameraPos.y, cameraPos.z);
	glUniform3fv(uObjColorLoc, 1, glm::value_ptr(uColor));
	glDrawElements(GL_TRIANGLES, fCount * 3, GL_UNSIGNED_INT, (void*)(0));
	glBindVertexArray(0);

	//std::cout << "Monster x : " << this->center.x << "//";
	//std::cout << "Monster x : " << this->center.z << "//" << std::endl;
}

void Monster::TakeDamage(float attack) {
	if (this->hp - attack <= 0.0f) {
		this->hp = 0.0f;
		this->isDestroyed = true;
	}
	else {
		this->hp -= attack;
	}
}

bool Monster::GetDestroyed() {
	return this->isDestroyed;
}

bool Monster::CollisionWithTarget() {
	glm::mat4 targetModelMat = this->target->GetModelMat();
	glm::mat4 inverseMat = glm::inverse(targetModelMat);
	glm::vec4 vec = inverseMat * glm::vec4(this->center, 1);
	glm::vec3 localCenter = glm::vec3(vec);
	glm::vec3 nearestPoint = glm::vec3(0, 0, 0);
	nearestPoint.x = glm::clamp(localCenter.x, -gTankSize_width / 2.0f, gTankSize_width / 2.0f);
	nearestPoint.y = glm::clamp(localCenter.y, -gTankSize_height / 2.0f, gTankSize_height / 2.0f);
	nearestPoint.z = glm::clamp(localCenter.z, -gTankSize_depth / 2.0f, gTankSize_depth / 2.0f);

	float distance =
		(nearestPoint.x - localCenter.x) * (nearestPoint.x - localCenter.x) +
		(nearestPoint.y - localCenter.y) * (nearestPoint.y - localCenter.y) +
		(nearestPoint.z - localCenter.z) * (nearestPoint.z - localCenter.z);

	if (distance <= this->boundRadius * this->boundRadius) {

		return true;
	}
	return false;
}

float Monster::GetBoundRadius() {
	return this->boundRadius;
}

void Monster::ApplyKnockback() {
	float knockbackDis = 1.0f; // 넉백될 거리 per frame
	// 뷰포인트는 몬스터 -> 탱크 방향이므로, 반대 방향은 -viewPoint
	glm::vec3 knockbackDir = -this->viewPoint;
	glm::vec3 knockbackVector = knockbackDir * knockbackDis;

	// 넉백을 몬스터의 위치(center)와 모델 행렬(modelMat)에 즉시 적용
	this->currKnockbackDis += knockbackDis;
	this->center += knockbackVector;
	this->modelMat = glm::translate(glm::mat4(1.0f), knockbackVector) * this->modelMat;

	if (maxKnockbackDis <= this->currKnockbackDis) {
		this->currKnockbackDis = 0.0f;
		this->isKnockbacking = false;
	}
}