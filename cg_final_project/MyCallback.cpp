#include <iostream>
#include <vector>
#include <string>
#include <cstdlib>
#include <ctime>
#include <algorithm> 
#include <gl/glew.h>
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h>
#include <gl/glm/glm.hpp>
#include <gl/glm/ext.hpp>
#include <gl/glm/gtc/matrix_transform.hpp>
#include "MyCallback.h"
#include "MyExtern.h"
#include "MyStruct.h"
#include "MyUtils.h"
#include "Tank.h"
#include "CameraMain.h"
#include "CameraSub.h"
#include "Plane.h"

GLvoid DrawScene() {
	float currentFrame = (float)glutGet(GLUT_ELAPSED_TIME) / 1000.0f;
	deltaTime = currentFrame - lastFrame;
	lastFrame = currentFrame;

	// 1. 시간 흐름 계산 (3분 = 180초)
	float maxTime = 180.0f;
	float ratio = currentFrame / maxTime;
	if (ratio > 1.0f) ratio = 1.0f;

	// 2. 조명 색상 (지형용)
	glm::vec3 dayLight(1.0f, 1.0f, 1.0f);       // 흰색 빛
	glm::vec3 sunsetLight(1.0f, 0.85f, 0.7f);    // 붉은 빛
	lightColor = glm::mix(dayLight, sunsetLight, ratio);

	// 3. 조명 위치 (해가 떨어짐)
	float sunHeight = glm::mix(500.0f, 100.0f, ratio); // 500에서 지평선 아래(-100)까지
	lightPos = glm::vec3(500.0f, sunHeight, 500.0f);

	// 4. [추가] 하늘 색상 (배경용)
	// 낮: 밝은 하늘색 (Sky Blue)
	glm::vec3 daySky(0.53f, 0.81f, 0.92f);
	// 노을: 진한 주황/보라색 (Sunset)
	glm::vec3 sunsetSky(0.7f, 0.3f, 0.1f);

	glm::vec3 currentSkyColor = glm::mix(daySky, sunsetSky, ratio);


	// --- 렌더링 시작 ---
	glClearColor(currentSkyColor.r, currentSkyColor.g, currentSkyColor.b, 1.0f); // 배경 초기화색도 맞춤
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
	// change view & projection matrix
	// draw main screen
	if (myMainCamera) {
		myMainCamera->ChangeViewMat();
		SetProjMatMain();
	}

	// 1. 지형 및 객체 렌더링
	glUseProgram(mainShader);
	glUniformMatrix4fv(glGetUniformLocation(mainShader, "view"), 1, GL_FALSE, glm::value_ptr(gViewMat));
	glUniformMatrix4fv(glGetUniformLocation(mainShader, "projection"), 1, GL_FALSE, glm::value_ptr(gProjMat));
	glUniform3fv(glGetUniformLocation(mainShader, "lightPos"), 1, glm::value_ptr(lightPos));
	glm::vec3 cameraPos = myMainCamera->GetEye();
	glUniform3fv(glGetUniformLocation(mainShader, "viewPos"), 1, glm::value_ptr(cameraPos));
	glUniform3fv(glGetUniformLocation(mainShader, "lightColor"), 1, glm::value_ptr(lightColor));

	glBindVertexArray(cubeVAO);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, grassTexture);

	GLint modelLoc = glGetUniformLocation(mainShader, "model");
	GLint uvScaleLoc = glGetUniformLocation(mainShader, "uvScale");

	for (const auto& chunk : renderList) {
		glm::mat4 model = glm::mat4(1.0f);
		float h = (float)chunk.height;
		float totalH = h + 1.0f;
		float centerY = (h / 2.0f) - 0.5f;

		model = glm::translate(model, glm::vec3(chunk.x, centerY, chunk.z));
		model = glm::scale(model, glm::vec3(chunk.w * BLOCK_SIZE, totalH, chunk.d * BLOCK_SIZE));

		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		glUniform2f(uvScaleLoc, chunk.w * 0.5f, chunk.d * 0.5f);
		glDrawArrays(GL_TRIANGLES, 0, 36);
	}

	//// draw plane (sample)
	//if (myPlane) {
	//	myPlane->DrawPlane("main");
	//}
	// draw tank
	if (myTank) {
		myTank->DrawAllPart("main");
	}

	// 2. 스카이박스 렌더링
	glDepthFunc(GL_LEQUAL);
	glUseProgram(skyboxShader);

	// [핵심] 계산된 하늘 색상을 셰이더로 전달
	glUniform3fv(glGetUniformLocation(skyboxShader, "skyColor"), 1, glm::value_ptr(currentSkyColor));

	glm::mat4 viewSky = glm::mat4(glm::mat3(gViewMat));
	glUniformMatrix4fv(glGetUniformLocation(skyboxShader, "view"), 1, GL_FALSE, glm::value_ptr(viewSky));
	glUniformMatrix4fv(glGetUniformLocation(skyboxShader, "projection"), 1, GL_FALSE, glm::value_ptr(gProjMat));

	glBindVertexArray(skyboxVAO);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTexture);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glDepthFunc(GL_LESS);


	//// draw mini map
	//glClear(GL_DEPTH_BUFFER_BIT);
	//glViewport(window_w - minimap_size, window_h - minimap_size, minimap_size, minimap_size);
	//// change view & projection matrix
	//if (mySubCamera) {
	//	mySubCamera->ChangeViewMat();
	//	SetProjMatSub();
	//}
	//// draw plane (sample)
	//if (myPlane) {
	//	myPlane->DrawPlane("sub");
	//}
	//// draw tank
	//if (myTank) {
	//	myTank->DrawAllPart("sub");
	//}

	glutSwapBuffers();
}

GLvoid Reshape(int w, int h) {
	SCR_WIDTH = w;
	SCR_HEIGHT = h;
}

GLvoid Keyboard(unsigned char key, int x, int y) {
	switch (key) {
	case 'w':
		if (myTank) myTank->SetIsFront(true);
		break;
	case 's':
		if (myTank) myTank->SetIsBack(true);
		break;
	case 'a':
		if (myTank) myTank->SetIsLeft(true);
		break;
	case 'd':
		if (myTank) myTank->SetIsRight(true);
		break;
	case 'q':
		glutLeaveMainLoop();
		break;
	}
	glutPostRedisplay();
}
GLvoid KeyboardUp(unsigned char key, int x, int y) {
	switch (key) {
	case 'w':
		if (myTank) myTank->SetIsFront(false);
		break;
	case 's':
		if (myTank) myTank->SetIsBack(false);
		break;
	case 'a':
		if (myTank) myTank->SetIsLeft(false);
		break;
	case 'd':
		if (myTank) myTank->SetIsRight(false);
		break;
	}
	glutPostRedisplay();
}

GLvoid SpecialKey(int key, int x, int y) {
	glutPostRedisplay();
}

GLvoid SpecialKeyUp(int key, int x, int y) {
	glutPostRedisplay();
}

GLvoid Timer(int value) {
	if (myTank) myTank->Update();
	if (myMainCamera) myMainCamera->UpdateVectors();
	if (mySubCamera) mySubCamera->UpdateVectors();
	glutPostRedisplay();
	glutTimerFunc(16, Timer, 0);
}

GLvoid Mouse(int button, int state, int x, int y) {

}
GLvoid MouseMotion(int x, int y) {

}