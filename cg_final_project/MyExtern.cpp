#include "MyExtern.h"
#include "MyStruct.h"
#include <vector>

GLuint vertexShader;
GLuint fragmentShader;
GLuint shaderProgramID;

int minimap_size = 300;

glm::mat4 gViewMat = glm::mat4(1.0);
glm::mat4 gProjMat = glm::mat4(1.0);

float gTankSize_width = 10.0f / 10.0f;
float gTankSize_height = 7.0f / 10.0f;
float gTankSize_depth = 20.0f / 10.0f;

//float gravity = 9.8f;

Plane* myPlane = nullptr;
Tank* myTank = nullptr;
std::vector<Monster*> myMonsters;
std::vector<Bullet*> myBullets;
Boss* myBoss = nullptr;
Stage* myStage = nullptr;
CameraMain* myMainCamera = nullptr;
CameraSub* mySubCamera = nullptr;



// --- 전역 변수 ---
unsigned int SCR_WIDTH = 1920;
unsigned int SCR_HEIGHT = 1080;

const int MAP_SIZE = 1000;
const float BLOCK_SIZE = 1.0f;
int mapHeight[1000][1000]; // 물리 충돌용 맵 데이터

std::vector<RenderChunk> renderList;

glm::vec3 objectPos = glm::vec3(0.0f, 10.0f, 0.0f);
float objectSpeed = 20.0f;
float playerSize = 0.8f;

// 물리
//float verticalVelocity = 0.0f;
//float gravity = 30.0f;
//float jumpForce = 10.0f;
//bool isGrounded = false;
//bool keyState[256];

// 카메라
glm::vec3 cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);
float cameraDistance = 30.0f;
float yaw = 45.0f;
float pitch = 35.0f;

float lastX = SCR_WIDTH / 2.0;
float lastY = SCR_HEIGHT / 2.0;
bool firstMouse = true;
float gDeltaTime = 0.0f;
float lastFrame = 0.0f;

// OpenGL 자원
GLuint mainShader, skyboxShader;
GLuint grassTexture, boxTexture, skyboxTexture;
GLuint cubeVAO, cubeVBO, skyboxVAO, skyboxVBO;

// 조명
glm::vec3 lightPos = glm::vec3(500.0f, 500.0f, 500.0f);
glm::vec3 lightColor = glm::vec3(1.0f, 1.0f, 1.0f);

bool gAssembleTime = false;
bool gAssembleActive = false;
int gAssembleCount = 0;

float gStageStart = 0.0f;
float gStageDuration = 10.0f;