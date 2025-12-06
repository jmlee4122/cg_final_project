#pragma once

#include <iostream>
#include <vector>
#include <gl/glew.h>
#include <gl/freeglut.h>
#include <gl/glm/glm.hpp>


class Bullet;
class Monster;
class Plane;
class Tank;
class CameraMain;
class CameraSub;


extern GLuint vertexShader;
extern GLuint fragmentShader;
extern GLuint shaderProgramID;

extern int minimap_size;

extern glm::mat4 gViewMat;
extern glm::mat4 gProjMat;

extern float gTankSize_width;
extern float gTankSize_height;
extern float gTankSize_depth;

//extern float gravity;

extern Plane* myPlane;
extern Tank* myTank;
extern std::vector<Monster*> myMonsters;
extern std::vector<Bullet*> myBullets;
extern CameraMain* myMainCamera;
extern CameraSub* mySubCamera;



struct RenderChunk;

// --- 전역 변수 ---
extern unsigned int SCR_WIDTH;
extern unsigned int SCR_HEIGHT;

// 맵 설정 (1000 x 1000)
extern const int MAP_SIZE;
extern const float BLOCK_SIZE;
extern int mapHeight[1000][1000]; // 물리 충돌용 맵 데이터

extern std::vector<RenderChunk> renderList;

extern glm::vec3 objectPos;
extern float objectSpeed;
extern float playerSize;

// 물리
//extern float verticalVelocity;
//extern float gravity;
//extern float jumpForce;
//extern bool isGrounded;
//extern bool keyState[256];

// 카메라
extern glm::vec3 cameraTarget;
extern float cameraDistance;
extern float yaw;
extern float pitch;

extern float lastX;
extern float lastY;
extern bool firstMouse;
extern float gDeltaTime;
extern float lastFrame;

// OpenGL 자원
extern GLuint mainShader, skyboxShader;
extern GLuint grassTexture, boxTexture, skyboxTexture;
extern GLuint cubeVAO, cubeVBO, skyboxVAO, skyboxVBO;

// 조명
extern glm::vec3 lightPos;
extern glm::vec3 lightColor;

// [UI 시스템 추가]
enum GameState {
    STATE_TITLE,
    STATE_PLAY,
    STATE_FAILURE
};

extern int currentScene;       // 현재 게임 상태
extern float gameStartTime;    // 게임이 실제로 시작된 시간 (타이틀에서 보낸 시간 제외)
extern GLuint titleTexture;    // 타이틀 화면 이미지
extern GLuint failTexture;

const int MAX_MONSTERS = 20;        // 최대 몬스터 수
const float SPAWN_RADIUS_MIN = 60.0f; // 최소 거리
const float SPAWN_RADIUS_MAX = 120.0f; // 최대 거리 (시야 밖 생성)
const int SPAWN_CHANCE = 5;  // 프레임당 생성 확률