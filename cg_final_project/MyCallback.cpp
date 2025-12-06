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
#include "Bullet.h"
#include "Monster.h"
#include "Boss.h"
#include "Stage.h"
#include "Ice.h"
#include "UserInterface.h"

int currentScene = STATE_TITLE;
float gameStartTime = 0.0f;


GLvoid DrawScene() {
	// 1. 타이틀 화면일 때
	if (currentScene == STATE_TITLE) {
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// 타이틀 그리기
		DrawTitleScreen(titleTexture, SCR_WIDTH, SCR_HEIGHT);

		glutSwapBuffers();
		return; // 게임 로직 실행 안 함
	}
	if (currentScene == STATE_FAILURE) {
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		DrawFailScreen(failTexture, SCR_WIDTH, SCR_HEIGHT); // 실패 화면 그리기

		glutSwapBuffers();
		return;
	}
	if (currentScene == STATE_CLEAR) {
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		DrawClearScreen(clearTexture, SCR_WIDTH, SCR_HEIGHT); // 클리어 화면 그리기

		glutSwapBuffers();
		return;
	}


	float currentTime = (float)glutGet(GLUT_ELAPSED_TIME) / 1000.0f;

	float inGameTime = currentTime - gameStartTime;
	gDeltaTime = currentTime - lastFrame;
	lastFrame = currentTime;

	if (inGameTime > boss_time && !gAssembleTime && !gAssembleActive) {
		gAssembleTime = true;
		gAssembleActive = true;
		for (auto r : myMonsters) {
			r->SpeedUp();
		}
	}


	// 1. 시간 흐름 계산 (3분 = 180초)
	float maxTime = boss_time;
	float ratio = inGameTime / maxTime;

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

	if (myTank) {
		myTank->DrawAllPart("main");
	}
	if (!myBullets.empty()) {
		for (auto r : myBullets) {
			r->Draw("main");
		}
	}
	if (!myMonsters.empty()) {
		for (auto r : myMonsters) {
			r->Draw("main");
		}
	}
	if (myBoss) {
		myBoss->Draw("main");
	}
	if (myStage) {
		myStage->DrawStage("main");
	}
	if (!myIces.empty()) {
		for (auto r : myIces) {
			r->Draw("main");
		}
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

	if (myTank && myTank->GetHP() <= 0.0f) {
		currentScene = STATE_FAILURE;
		glutSetCursor(GLUT_CURSOR_INHERIT);
	}

	// 3. 인게임 UI 그리기
	if (myTank && !myBoss) {
		// Tank 클래스에 Getter가 없으면 Tank.h에 float GetHp() { return hp; } 추가 필요
		// 여기서는 임시로 hp에 접근한다고 가정하거나 getter 사용
		DrawInGameUI(inGameTime,myTank->GetHP(),100.0f , 0, 0, SCR_WIDTH, SCR_HEIGHT);
	}
	else if (myTank && myBoss) {
		// Tank 클래스에 Getter가 없으면 Tank.h에 float GetHp() { return hp; } 추가 필요
		// 여기서는 임시로 hp에 접근한다고 가정하거나 getter 사용
		DrawInGameUI(inGameTime, myTank->GetHP(), 100.0f, myBoss->GetBossHP(), myBoss->GetBossMaxHP(), SCR_WIDTH, SCR_HEIGHT);
	}
	glutSwapBuffers();
}

GLvoid Reshape(int w, int h) {
	SCR_WIDTH = w;
	SCR_HEIGHT = h;
}

GLvoid Keyboard(unsigned char key, int x, int y) {
	// 타이틀 화면에서 스페이스바 처리
	if (currentScene == STATE_TITLE) {
		if (key == ' ') {
			currentScene = STATE_PLAY;
			gameStartTime = (float)glutGet(GLUT_ELAPSED_TIME) / 1000.0f;

			ResetGame();

			glutSetCursor(GLUT_CURSOR_NONE);
		}
		return;
	}
	//실패 화면에서 스페이스바 -> 타이틀로 이동
	if (currentScene == STATE_FAILURE) {
		if (key == ' ') {
			currentScene = STATE_TITLE; // 타이틀로 돌아감
		}
		return;
	}
	if (currentScene == STATE_CLEAR) {
		if (key == ' ') {
			currentScene = STATE_TITLE; // 타이틀로 복귀
		}
		return;
	}
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
	case ' ':
		if (myTank) myTank->SetIsJumping(true);
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
	if (currentScene != STATE_PLAY) {
		glutPostRedisplay();
		glutTimerFunc(16, Timer, 0);
		return;
	}
	if (gIsRunning) {
		if (myTank) myTank->Update();
		if (!myBullets.empty()) {
			for (auto r : myBullets) {
				r->Update();
			}
		}
		if (!myMonsters.empty()) {
			for (auto r : myMonsters) {
				r->Update();
			}
		}
		ManageMonsterSpawning();
		if (myBoss) myBoss->Update();
		if (myStage) {
			float currTime = (float)glutGet(GLUT_ELAPSED_TIME) / 1000.0f;
			if (currTime - gStageStart >= gStageDuration) {
				delete myStage;
				myStage = nullptr;
			}
		}
		if (!myIces.empty()) {
			for (auto r : myIces) {
				r->Update();
			}
		}
		if (myMainCamera) myMainCamera->UpdateVectors();

		if (gAssembleActive && gAssembleTime) {
			for (auto r : myMonsters) {
				if (r->IsAtOrigin()) {
					r->SetDestroyed(true);
					gAssembleCount += 1;
					// 처음으로 기본 몬스터가 원점에 도달한 경우
					// boss 몬스터를 생성
					if (gAssembleCount == 1 && myBoss == nullptr) {
						CreateBoss();
					}
				}
			}

			if (gAssembleCount >= 1 && myBoss != nullptr) {
				myBoss->IncreaseSize(gAssembleCount);
			}

			if (myMonsters.empty()) {
				gAssembleActive = false;
				// std::cout << "### All assembled ###" << std::endl;
				std::cout << "basic monster count : " << gAssembleCount << std::endl;
			}
		}

		RemoveDestroyed(myBullets);
		RemoveDestroyed(myMonsters);
		RemoveDestroyed(myIces);
		if (myBoss) {
			if (myBoss->GetDestroyed()) {
				delete myBoss;
				myBoss = nullptr;
				currentScene = STATE_CLEAR;
				glutSetCursor(GLUT_CURSOR_INHERIT);
			}
		}

		glutPostRedisplay();
		glutTimerFunc(16, Timer, 0);
	}
}

GLvoid Mouse(int button, int state, int x, int y) {
	if (button == GLUT_LEFT_BUTTON) {
		if (state == GLUT_DOWN) {
			myTank->attack();
		}
	}
	glutPostRedisplay();
}
GLvoid MouseMotion(int x, int y) {

}

GLvoid PassiveMotion(int x, int y) {
	// 화면 중앙 좌표 계산
	int centerX = SCR_WIDTH / 2;
	int centerY = SCR_HEIGHT / 2;

	// 1. 만약 마우스가 이미 중앙에 있다면 계산할 필요 없음 
	// (glutWarpPointer로 인해 호출된 경우 무시)
	if (x == centerX && y == centerY) return;

	// 2. 중앙을 기준으로 얼마나 움직였는지 차이(Offset) 계산
	float xoffset = (float)(x - centerX);
	float yoffset = (float)(centerY - y); // Y좌표는 위로 갈수록 작아지므로 반대로 계산

	// 3. 감도 적용 및 각도 업데이트
	float sensitivity = 0.1f;
	xoffset *= sensitivity;
	yoffset *= sensitivity;

	// 마우스 움직임 방향대로 카메라 회전 (이전 요청 반영: -= 사용)
	yaw -= xoffset;
	pitch -= yoffset;

	// 각도 제한
	if (pitch > 89.0f) pitch = 89.0f;
	if (pitch < 5.0f) pitch = 5.0f;

	// 4. [핵심] 마우스를 다시 화면 중앙으로 강제 이동
	glutWarpPointer(centerX, centerY);
}