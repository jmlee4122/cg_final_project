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

extern bool gAssembleTime;
extern bool gAssembleActive;