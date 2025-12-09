// UserInterface.cpp
#include "UserInterface.h"
#include "MyExtern.h"
#include <cstdio>

void Begin2D() {
    glDisable(GL_DEPTH_TEST); // UI는 깊이 검사 끄기 (항상 위에 그림)
    glDisable(GL_LIGHTING);   // 조명 끄기 (원래 색 그대로)

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    // 좌표계: 왼쪽 아래(0,0) ~ 오른쪽 위(1, 1) 로 설정 (비율 계산 쉽게)
    // 혹은 픽셀 단위로 하려면 gluOrtho2D(0, width, 0, height)를 사용
    // 여기서는 편의상 GLUT 좌표계와 맞추거나 정규화 좌표를 쓸 수 있는데,
    // 직관적인 픽셀 매칭을 위해 외부에서 width/height를 받아오는 방식으로 처리
}

void End2D() {
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);

    glEnable(GL_LIGHTING);
    glEnable(GL_DEPTH_TEST);
}

void RenderText(float x, float y, std::string text, void* font, glm::vec3 color) {
    glColor3f(color.r, color.g, color.b);
    glRasterPos2f(x, y); // 텍스트 시작 위치
    for (char c : text) {
        glutBitmapCharacter(font, c);
    }
}

void RenderLargeText(float x, float y, std::string text, float scale, glm::vec3 color) {
    // 1. 색상 및 두께 설정
    glColor3f(color.r, color.g, color.b);
    glLineWidth(2.0f); // 글씨 두께 (2.0f 정도로 하면 적당히 굵어짐)

    // 2. 위치 및 크기 변환을 위한 행렬 저장
    glPushMatrix();

    // 3. 위치 이동 (Stroke 폰트는 glRasterPos가 아니라 translate로 이동함)
    glTranslatef(x, y, 0.0f);

    // 4. 크기 조절 (scale 값으로 폰트 크기 조정)
    // 기본 Stroke 폰트가 매우 크기 때문에 보통 0.2 ~ 0.5 정도로 줄여서 사용함
    glScalef(scale, scale, scale);

    // 5. 글자 그리기 (GLUT_STROKE_ROMAN 사용)
    for (char c : text) {
        glutStrokeCharacter(GLUT_STROKE_ROMAN, c);
    }

    // 6. 행렬 복구
    glPopMatrix();
    glLineWidth(1.0f); // 두께 원상복구
}

void DrawRect(float x, float y, float w, float h, glm::vec3 color) {
    glColor3f(color.r, color.g, color.b);
    glBegin(GL_QUADS);
    glVertex2f(x, y);
    glVertex2f(x + w, y);
    glVertex2f(x + w, y + h);
    glVertex2f(x, y + h);
    glEnd();
}

void DrawTitleScreen(GLuint texID, int scrWidth, int scrHeight) {
    glUseProgram(0);

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);

    // 전체 화면 텍스처 그리기
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(0, scrWidth, 0, scrHeight, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // 텍스처가 로드되지 않았을 경우를 대비해 흰색 바탕 확인
    if (texID == 0) {
        // 이미지가 없으면 빨간색으로라도 칠해서 화면이 나오는지 테스트
        glColor3f(1.0f, 0.0f, 0.0f);
    }
    else {
        glColor3f(1, 1, 1);
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, texID);
    }

    glBegin(GL_QUADS);
    glTexCoord2f(0, 0); glVertex2f(0, 0);
    glTexCoord2f(1, 0); glVertex2f((float)scrWidth, 0);
    glTexCoord2f(1, 1); glVertex2f((float)scrWidth, (float)scrHeight);
    glTexCoord2f(0, 1); glVertex2f(0, (float)scrHeight);
    glEnd();

    if (texID != 0) glDisable(GL_TEXTURE_2D);

 
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glEnable(GL_DEPTH_TEST);
}
void DrawFailScreen(GLuint texID, int scrWidth, int scrHeight) {
    glUseProgram(0);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);

    // 배경 그리기 (DrawTitleScreen과 동일)
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(0, scrWidth, 0, scrHeight, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    if (texID == 0) {
        glColor3f(0.5f, 0.0f, 0.0f); // 이미지가 없으면 어두운 빨강 배경
    }
    else {
        glColor3f(1, 1, 1);
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, texID);
    }

    glBegin(GL_QUADS);
    glTexCoord2f(0, 0); glVertex2f(0, 0);
    glTexCoord2f(1, 0); glVertex2f((float)scrWidth, 0);
    glTexCoord2f(1, 1); glVertex2f((float)scrWidth, (float)scrHeight);
    glTexCoord2f(0, 1); glVertex2f(0, (float)scrHeight);
    glEnd();

    if (texID != 0) glDisable(GL_TEXTURE_2D);
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glEnable(GL_DEPTH_TEST);
}
void DrawClearScreen(GLuint texID, int scrWidth, int scrHeight) {
    glUseProgram(0);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);

    // 배경 그리기 (다른 화면들과 동일)
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(0, scrWidth, 0, scrHeight, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    if (texID == 0) {
        glColor3f(0.0f, 0.0f, 0.5f); // 이미지가 없으면 파란색 배경
    }
    else {
        glColor3f(1, 1, 1);
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, texID);
    }

    glBegin(GL_QUADS);
    glTexCoord2f(0, 0); glVertex2f(0, 0);
    glTexCoord2f(1, 0); glVertex2f((float)scrWidth, 0);
    glTexCoord2f(1, 1); glVertex2f((float)scrWidth, (float)scrHeight);
    glTexCoord2f(0, 1); glVertex2f(0, (float)scrHeight);
    glEnd();

    if (texID != 0) glDisable(GL_TEXTURE_2D);


    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glEnable(GL_DEPTH_TEST);
}

void DrawInGameUI(float time, float currentHp, float maxHp,float boss_currentHp, float boss_maxHp, int scrWidth, int scrHeight) {
    glUseProgram(0);
    // 2D 모드 진입 (좌표계: 0~Width, 0~Height)
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(0, scrWidth, 0, scrHeight, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);

    // 1. 좌측 상단: 타이머
    if (time < boss_time) {
        int min = (int)time / 60;
        int sec = (int)time % 60;
        char buf[100];
        sprintf_s(buf, "Time: %02d:%02d", min, sec);
        RenderText(20, scrHeight - 40, buf, GLUT_BITMAP_TIMES_ROMAN_24, glm::vec3(1, 1, 1));
    }
    // 2. 3분 이후: 보스 HP바 (상단 중앙)
    else {
        float barW = 600.0f;
        float barH = 30.0f;
        float barX = (scrWidth - barW) / 2.0f;
        float barY = scrHeight - 60.0f;

        // 배경 (회색)
        DrawRect(barX, barY, barW, barH, glm::vec3(0.3f, 0.3f, 0.3f));

        // 보스 체력 비율 계산
        float bossRatio = boss_currentHp / boss_maxHp;
        if (bossRatio < 0) bossRatio = 0;

        // 체력 (빨강) - 현재는 100%로 고정
        DrawRect(barX, barY, barW * bossRatio, barH, glm::vec3(0.8f, 0.0f, 0.0f));


        // 텍스트
        RenderText(barX + barW / 2.0f - 40, barY + 5, "BOSS", GLUT_BITMAP_HELVETICA_18, glm::vec3(1, 1, 1));
    }

    // 3. 하단: 플레이어 탱크 HP
    {
        float barW = 400.0f;
        float barH = 20.0f;
        float barX = 20.0f; // 왼쪽 하단
        float barY = 20.0f;

        // 배경
        DrawRect(barX, barY, barW, barH, glm::vec3(0.2f, 0.2f, 0.2f));

        // 현재 체력 비율
        float ratio = currentHp / maxHp;
        if (ratio < 0) ratio = 0;

        // 체력 (초록 -> 노랑 -> 빨강 논리도 가능하지만 일단 초록)
        DrawRect(barX, barY, barW * ratio, barH, glm::vec3(0.0f, 0.8f, 0.0f));

        char buf[50];
        sprintf_s(buf, "HP: %.0f / %.0f", currentHp, maxHp);
        RenderText(barX + 10, barY + 25, buf, GLUT_BITMAP_HELVETICA_18, glm::vec3(1, 1, 1));
    }

    // 복구
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING); // 조명 다시 켜주기
}