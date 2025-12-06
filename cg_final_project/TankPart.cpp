#include <iostream>
#include <vector>
#include <gl/glew.h>
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h>
#include <gl/glm/glm.hpp>
#include <gl/glm/ext.hpp>
#include <gl/glm/gtc/matrix_transform.hpp>
#include "TankPart.h"
#include "MyUtils.h"
#include "MyExtern.h"
#include "MyStruct.h"
#include "CameraMain.h"
#include "CameraSub.h"

TankPart::TankPart(Model* model, std::string name) : VAO(0), VBO_pos(0), VBO_nol(0), EBO(0) {
	this->model = model;
	this->vCount = model->vertex_count, this->fCount = model->face_count;
	//this->modelMat = glm::mat4(1.0);
	this->modelMat = glm::scale(glm::mat4(1.0), glm::vec3(0.1f, 0.1f, 0.1f));
	this->uColor = glm::vec3(0.0f, 0.0f, 0.0f);
	this->uLightColorLoc = 0, this->uLightPosLoc = 0, this->uViewPosLoc = 0, this->uObjColorLoc = 0;
	this->uProjLoc = 0, this->uViewLoc = 0, this->uModelLoc = 0;
	this->name = name;
	this->pChild = nullptr;
	// 노말 데이터가 있는지 확인
	if (model->normals == nullptr) {
		std::cerr << "ERROR: Model normals are not loaded!" << std::endl;
		return;
	}

	InitBuffers(VAO, VBO_pos, VBO_nol, EBO, model->vertices, model->normals,
		model->faces, vCount, fCount, model->normal_count);  // normal_count 추가
	SetColor();

	glUseProgram(shaderProgramID);

	this->uLightPosLoc = glGetUniformLocation(shaderProgramID, "lightPos");
	this->uLightColorLoc = glGetUniformLocation(shaderProgramID, "lightColor");


	this->uModelLoc = glGetUniformLocation(shaderProgramID, "model");
	this->uViewLoc = glGetUniformLocation(shaderProgramID, "view");
	this->uProjLoc = glGetUniformLocation(shaderProgramID, "projection");

	this->uViewPosLoc = glGetUniformLocation(shaderProgramID, "viewPos");
	this->uObjColorLoc = glGetUniformLocation(shaderProgramID, "objectColor");
}
TankPart::~TankPart() {
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

void TankPart::SetColor() {
	if (name == "bottom") 
		this->uColor = glm::vec3(0.2f, 0.2f, 0.2f);
	else if (name == "mid") 
		this->uColor = glm::vec3(0.3f, 0.4f, 0.3f);
	else if (name == "top") 
		this->uColor = glm::vec3(0.4f, 0.6f, 0.4f);
	else if (name == "barrel") 
		this->uColor = glm::vec3(0.5f, 0.7f, 0.5f);
	else 
		std::cout << "error: wrong name" << std::endl;
}

void TankPart::SetModelMat(glm::mat4 m) { // recursion
	this->modelMat = m * this->modelMat;
	if (this->pChild == nullptr) return;
	this->pChild->SetModelMat(m);
}

void TankPart::DrawPart(std::string str) { // recursion
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
	if (str == "main")
		cameraPos = myMainCamera->GetEye();
	else if (str == "sub")
		cameraPos = mySubCamera->GetEye();
	
	glUniform3f(uViewPosLoc, cameraPos.x, cameraPos.y, cameraPos.z);
	glUniform3fv(uObjColorLoc, 1, glm::value_ptr(uColor));
	glDrawElements(GL_TRIANGLES, fCount * 3, GL_UNSIGNED_INT, (void*)(0));
	glBindVertexArray(0);

	// recursion
	if (this->pChild == nullptr) return;
	this->pChild->DrawPart(str);
}

void TankPart::SetChild(TankPart* child) {
	this->pChild = child;
}