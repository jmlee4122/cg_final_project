#pragma once
#include <gl/glew.h>
#include <gl/freeglut.h>
#include <gl/glm/glm.hpp>
#include <string>

// 2D 그리기 모드로 전환/복구하는 헬퍼 함수
void Begin2D();
void End2D();

// 텍스트 그리기
void RenderText(float x, float y, std::string text, void* font = GLUT_BITMAP_HELVETICA_18, glm::vec3 color = glm::vec3(1, 1, 1));

// 사각형 그리기 (HP바 등)
void DrawRect(float x, float y, float w, float h, glm::vec3 color);

// 전체 UI 그리기 함수
void DrawTitleScreen(GLuint texID, int scrWidth, int scrHeight);
void DrawFailScreen(GLuint texID, int scrWidth, int scrHeight);
void DrawInGameUI(float time, float currentHp, float maxHp, int scrWidth, int scrHeight);