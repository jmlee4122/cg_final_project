#define _CRT_SECURE_NO_WARNINGS
#define MAX_LINE_LENGTH 1200
#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <ctime>
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
#include "CameraSub.h"
#include "Tank.h"
#include "Plane.h"
#include "Monster.h"



void read_newline(char* str) {
	char* pos;
	if ((pos = strchr(str, '\n')) != NULL)
		*pos = '\0';
}
void read_obj_file(const char* filename, Model* model) {
	FILE* file;
	fopen_s(&file, filename, "r");
	if (!file) {
		perror("Error opening file");
		exit(EXIT_FAILURE);
	}
	char line[MAX_LINE_LENGTH];
	model->vertex_count = 0;
	model->face_count = 0;
	model->normal_count = 0;

	// 첫 번째 패스: 개수 세기
	while (fgets(line, sizeof(line), file)) {
		read_newline(line);
		if (line[0] == 'v' && line[1] == ' ')
			model->vertex_count++;
		else if (line[0] == 'f' && line[1] == ' ')
			model->face_count++;
		else if (line[0] == 'v' && line[1] == 'n' && line[2] == ' ')
			model->normal_count++;
	}

	fseek(file, 0, SEEK_SET);

	// 메모리 할당
	model->vertices = (Vertex*)malloc(model->vertex_count * sizeof(Vertex));
	model->faces = (Face*)malloc(model->face_count * sizeof(Face));
	model->normals = (Vertex*)malloc(model->normal_count * sizeof(Vertex));

	size_t vertex_index = 0;
	size_t face_index = 0;
	size_t normal_index = 0;

	// 두 번째 패스: 데이터 읽기
	while (fgets(line, sizeof(line), file)) {
		read_newline(line);
		if (line[0] == 'v' && line[1] == ' ') {
			sscanf_s(line + 2, "%f %f %f",
				&model->vertices[vertex_index].x,
				&model->vertices[vertex_index].y,
				&model->vertices[vertex_index].z);
			vertex_index++;
		}
		else if (line[0] == 'v' && line[1] == 'n' && line[2] == ' ') {
			sscanf_s(line + 2, "%f %f %f",
				&model->normals[normal_index].x,
				&model->normals[normal_index].y,
				&model->normals[normal_index].z);
			normal_index++;
		}
		else if (line[0] == 'f' && line[1] == ' ') {
			unsigned int v1, v2, v3, vn1, vn2, vn3;
			// f 1//1 2//2 3//3 형식 파싱
			int parsed = sscanf_s(line + 2, "%u//%u %u//%u %u//%u",
				&v1, &vn1, &v2, &vn2, &v3, &vn3);

			if (parsed == 6) {
				// v//vn 형식
				model->faces[face_index].v1 = v1 - 1;
				model->faces[face_index].v2 = v2 - 1;
				model->faces[face_index].v3 = v3 - 1;
				model->faces[face_index].vn1 = vn1 - 1;
				model->faces[face_index].vn2 = vn2 - 1;
				model->faces[face_index].vn3 = vn3 - 1;
			}
			else {
				// v 형식 (노말 없음)
				sscanf_s(line + 2, "%u %u %u", &v1, &v2, &v3);
				model->faces[face_index].v1 = v1 - 1;
				model->faces[face_index].v2 = v2 - 1;
				model->faces[face_index].v3 = v3 - 1;
				model->faces[face_index].vn1 = 0;
				model->faces[face_index].vn2 = 0;
				model->faces[face_index].vn3 = 0;
			}
			face_index++;
		}
	}
	fclose(file);
}
void print_model_info(const Model* model) {
	std::cout << "========== Model Information ==========" << std::endl;
	std::cout << "Vertex Count: " << model->vertex_count << std::endl;
	std::cout << "Normal Count: " << model->normal_count << std::endl;
	std::cout << "Face Count: " << model->face_count << std::endl;
	std::cout << std::endl;

	std::cout << "---------- Vertices ----------" << std::endl;
	for (size_t i = 0; i < model->vertex_count; ++i) {
		std::cout << "v[" << i << "]: ("
			<< model->vertices[i].x << ", "
			<< model->vertices[i].y << ", "
			<< model->vertices[i].z << ")" << std::endl;
	}
	std::cout << std::endl;

	std::cout << "---------- Normals ----------" << std::endl;
	if (model->normals != nullptr && model->normal_count > 0) {
		for (size_t i = 0; i < model->normal_count; ++i) {
			std::cout << "vn[" << i << "]: ("
				<< model->normals[i].x << ", "
				<< model->normals[i].y << ", "
				<< model->normals[i].z << ")" << std::endl;
		}
	}
	else {
		std::cout << "No normals loaded." << std::endl;
	}
	std::cout << std::endl;

	std::cout << "---------- Faces ----------" << std::endl;
	for (size_t i = 0; i < model->face_count; ++i) {
		std::cout << "f[" << i << "]: v("
			<< model->faces[i].v1 << ", "
			<< model->faces[i].v2 << ", "
			<< model->faces[i].v3 << ") vn("
			<< model->faces[i].vn1 << ", "
			<< model->faces[i].vn2 << ", "
			<< model->faces[i].vn3 << ")" << std::endl;
	}
	std::cout << "=======================================" << std::endl;
}

void InitBuffers(GLuint& VAO, GLuint& VBO_pos, GLuint& VBO_nol, GLuint& EBO,
	Vertex* vData, Vertex* vNormal, Face* fData, size_t vCount, size_t fCount, size_t nCount) {

	// 면마다 정점과 노말을 확장 (인덱스 기반 → 직접 데이터)
	std::vector<Vertex> expandedVertices;
	std::vector<Vertex> expandedNormals;
	std::vector<unsigned int> indices;

	for (size_t i = 0; i < fCount; ++i) {
		// 삼각형의 세 정점
		expandedVertices.push_back(vData[fData[i].v1]);
		expandedVertices.push_back(vData[fData[i].v2]);
		expandedVertices.push_back(vData[fData[i].v3]);

		// 해당하는 노말
		expandedNormals.push_back(vNormal[fData[i].vn1]);
		expandedNormals.push_back(vNormal[fData[i].vn2]);
		expandedNormals.push_back(vNormal[fData[i].vn3]);

		// 인덱스
		indices.push_back(i * 3 + 0);
		indices.push_back(i * 3 + 1);
		indices.push_back(i * 3 + 2);
	}

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO_pos);
	glGenBuffers(1, &VBO_nol);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_pos);
	glBufferData(GL_ARRAY_BUFFER, expandedVertices.size() * sizeof(Vertex),
		expandedVertices.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_nol);
	glBufferData(GL_ARRAY_BUFFER, expandedNormals.size() * sizeof(Vertex),
		expandedNormals.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int),
		indices.data(), GL_STATIC_DRAW);

	glBindVertexArray(0);
}
char* filetobuf(const char* file)
{
	FILE* fptr;
	long length;
	char* buf;
	fptr = fopen(file, "rb"); // Open file for reading
	if (!fptr) // Return NULL on failure
		return NULL;
	fseek(fptr, 0, SEEK_END); // Seek to the end of the file
	length = ftell(fptr); // Find out how many bytes into the file we are
	buf = (char*)malloc(length + 1); // Allocate a buffer for the entire length of the file and a null terminator
	fseek(fptr, 0, SEEK_SET); // Go back to the beginning of the file
	fread(buf, length, 1, fptr); // Read the contents of the file in to the buffer
	fclose(fptr); // Close the file
	buf[length] = 0; // Null terminator
	return buf; // Return the buffer
}
void make_vertexShaders()
{
	GLchar* vertexSource;
	//--- 버텍스 세이더 읽어 저장하고 컴파일 하기
	//--- filetobuf: 사용자정의 함수로 텍스트를 읽어서 문자열에 저장하는 함수
	vertexSource = filetobuf("vertex.glsl");
	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexSource, NULL);
	glCompileShader(vertexShader);
	GLint result;
	GLchar errorLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &result);
	if (!result)
	{
		glGetShaderInfoLog(vertexShader, 512, NULL, errorLog);
		std::cerr << "ERROR: vertex shader 컴파일 실패\n" << errorLog << std::endl;
		return;
	}
}
void make_fragmentShaders()
{
	GLchar* fragmentSource;
	//--- 프래그먼트 세이더 읽어 저장하고 컴파일하기
	fragmentSource = filetobuf("fragment.glsl"); // 프래그세이더 읽어오기
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
	glCompileShader(fragmentShader);
	GLint result;
	GLchar errorLog[512];
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &result);
	if (!result)
	{
		glGetShaderInfoLog(fragmentShader, 512, NULL, errorLog);
		std::cerr << "ERROR: frag_shader 컴파일 실패\n" << errorLog << std::endl;
		return;
	}
}
GLuint make_shaderProgram()
{
	GLint result;
	GLchar* errorLog = NULL;
	GLuint shaderID;
	shaderID = glCreateProgram(); //--- 세이더 프로그램 만들기
	glAttachShader(shaderID, vertexShader); //--- 세이더 프로그램에 버텍스 세이더 붙이기
	glAttachShader(shaderID, fragmentShader); //--- 세이더 프로그램에 프래그먼트 세이더 붙이기
	glLinkProgram(shaderID); //--- 세이더 프로그램 링크하기
	glDeleteShader(vertexShader); //--- 세이더 객체를 세이더 프로그램에 링크했음으로, 세이더 객체 자체는 삭제 가능
	glDeleteShader(fragmentShader);
	glGetProgramiv(shaderID, GL_LINK_STATUS, &result); // ---세이더가 잘 연결되었는지 체크하기
	if (!result) {
		glGetProgramInfoLog(shaderID, 512, NULL, errorLog);
		std::cerr << "ERROR: shader program 연결 실패\n" << errorLog << std::endl;
		return false;
	}
	glUseProgram(shaderID); //--- 만들어진 세이더 프로그램 사용하기
	//--- 여러 개의 세이더프로그램 만들 수 있고, 그 중 한개의 프로그램을 사용하려면
	//--- glUseProgram 함수를 호출하여 사용 할 특정 프로그램을 지정한다.
	//--- 사용하기 직전에 호출할 수 있다.
	return shaderID;
}

void SetProjMatMain() {
	float fovy = glm::radians(60.0f);
	float aspect = (float)SCR_WIDTH / (float)SCR_HEIGHT;
	float zNear = 0.1f;
	float zFar = 1000.0f;
	gProjMat = glm::perspective(fovy, aspect, zNear, zFar);
}
void SetProjMatSub() {
	float targetSize = gTankSize_depth;
	float orthoHalfSize = targetSize * 10.0f;
	float left = -orthoHalfSize;
	float right = orthoHalfSize;
	float bottom = -orthoHalfSize;
	float top = orthoHalfSize;
	float zNear = 0.0f;
	float zFar = mySubCamera->GetHeight() * 1.2f;
	gProjMat = glm::ortho(left, right, bottom, top, zNear, zFar);
}

void CreatePlane() {
	// create plane (sample)
	Model* planeModel = new Model;
	read_obj_file("plane_100.obj", planeModel);
	myPlane = new Plane(planeModel);
}
void CreateTank() {
	// Tank constructor param
	Model* bottomModel = new Model;
	read_obj_file("tank_bottom.obj", bottomModel);

	Model* midModel = new Model;
	read_obj_file("tank_mid.obj", midModel);

	Model* topModel = new Model;
	read_obj_file("tank_top.obj", topModel);

	Model* barrelModel = new Model;
	read_obj_file("tank_barrel.obj", barrelModel);

	// create tank
	myTank = new Tank(bottomModel, midModel, topModel, barrelModel);
}

glm::vec3 GetRandomSpawnPos() {
	if (!myTank) return glm::vec3(0, 0, 0);

	glm::vec3 playerPos = myTank->GetCenter();

	// 1. 랜덤 각도 (0 ~ 360도)
	float angle = (float)(rand() % 360);
	float radian = glm::radians(angle);

	// 2. 랜덤 거리 (Min ~ Max 사이)
	// rand() % N 은 정수만 나오므로, 100을 곱해서 랜덤을 돌리고 다시 100으로 나눔
	int minR = (int)(SPAWN_RADIUS_MIN * 10.0f);
	int maxR = (int)(SPAWN_RADIUS_MAX * 10.0f);
	float distance = (float)(minR + rand() % (maxR - minR)) / 10.0f;

	// 3. X, Z 좌표 계산 (원형 좌표계)
	float x = playerPos.x + distance * cos(radian);
	float z = playerPos.z + distance * sin(radian);

	// 4. 맵 밖으로 나가지 않게 보정 (맵 크기 MAP_SIZE 고려)
	// 맵 좌표는 -500 ~ 500 (BLOCK_SIZE가 1.0일 때)
	float limit = (MAP_SIZE / 2.0f) * BLOCK_SIZE - 5.0f; // 가장자리는 피함
	if (x > limit) x = limit;
	if (x < -limit) x = -limit;
	if (z > limit) z = limit;
	if (z < -limit) z = -limit;

	// 5. 지형 높이 가져오기
	float y = GetTerrainHeight(x, z);

	return glm::vec3(x, y, z);
}

// [추가] 몬스터 스폰 관리자
void ManageMonsterSpawning() {
	// 게임 중이 아니거나 탱크가 없으면 패스
	if (currentScene != STATE_PLAY || myTank == nullptr) return;

	// 현재 몬스터 수가 최대치보다 적을 때만 생성 시도
	if (myMonsters.size() < MAX_MONSTERS) {

		// 매 프레임 무조건 생성하면 20마리가 1초만에 팍! 생김
		// 약간의 랜덤성을 두어 "순차적"으로 생성되게 함 (예: 5% 확률)
		if ((rand() % 100) < SPAWN_CHANCE) {
			glm::vec3 spawnPos = GetRandomSpawnPos();
			CreateMonster(spawnPos);
			// std::cout << "Monster Spawned! Total: " << myMonsters.size() << std::endl;
		}
	}
}
void CreateMonster(glm::vec3 initLoc) {
	if (myTank == nullptr) {
		std::cerr << "ERROR: myTank is nullptr!" << std::endl;
		return;
	}
	std::cout << "myTank is valid" << std::endl;
	Model* monsterModel = new Model;
	read_obj_file("monster.obj", monsterModel);
	myMonsters.push_back(new Monster(monsterModel, myTank, initLoc));
}



// --- 유틸리티 함수들 ---
GLuint LoadBitmapTexture(const char* imagepath) {
	unsigned char header[54];
	unsigned int dataPos, width, height, imageSize;
	unsigned char* data;

	FILE* file = fopen(imagepath, "rb");
	if (!file) { return 0; }
	if (fread(header, 1, 54, file) != 54) { fclose(file); return 0; }
	if (header[0] != 'B' || header[1] != 'M') { fclose(file); return 0; }

	dataPos = *(int*)&(header[0x0A]);
	imageSize = *(int*)&(header[0x22]);
	width = *(int*)&(header[0x12]);
	height = *(int*)&(header[0x16]);

	if (imageSize == 0) imageSize = width * height * 3;
	if (dataPos == 0) dataPos = 54;

	data = new unsigned char[imageSize];
	fread(data, 1, imageSize, file);
	fclose(file);

	GLuint textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_BGR, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);
	delete[] data;
	return textureID;
}
GLuint CreateColorCubemap(int r, int g, int b) {
	GLuint textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
	GLubyte data[3] = { (GLubyte)r, (GLubyte)g, (GLubyte)b };
	for (unsigned int i = 0; i < 6; i++)
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, 1, 1, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	return textureID;
}
std::string LoadShaderSource(const char* filename) {
	std::ifstream file(filename);
	if (!file.is_open()) return "";
	std::stringstream buffer;
	buffer << file.rdbuf();
	return buffer.str();
}
GLuint CreateShaderProgram(const char* vsFile, const char* fsFile) {
	std::string vsSrc = LoadShaderSource(vsFile);
	std::string fsSrc = LoadShaderSource(fsFile);
	if (vsSrc.empty() || fsSrc.empty()) return 0;
	const char* vCode = vsSrc.c_str(); const char* fCode = fsSrc.c_str();
	GLuint vs = glCreateShader(GL_VERTEX_SHADER); glShaderSource(vs, 1, &vCode, NULL); glCompileShader(vs);
	GLuint fs = glCreateShader(GL_FRAGMENT_SHADER); glShaderSource(fs, 1, &fCode, NULL); glCompileShader(fs);
	GLuint program = glCreateProgram(); glAttachShader(program, vs); glAttachShader(program, fs); glLinkProgram(program);
	glDeleteShader(vs); glDeleteShader(fs);
	return program;
}
void SetupObjects() {
	float cubeVertices[] = {
		// Pos                 // Normal           // TexCoords
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f, 0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f, 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
		0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f, -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f, -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f, 0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 0.0f, 0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
		0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f, -0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 1.0f, -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,
		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f, -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 1.0f, -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f, -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f, -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f, 0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f, 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f, 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f, -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f, -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f, 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f, -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f, -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f
	};
	glGenVertexArrays(1, &cubeVAO); glGenBuffers(1, &cubeVBO);
	glBindVertexArray(cubeVAO); glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0); glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float))); glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float))); glEnableVertexAttribArray(2);

	float skyboxVertices[] = { /* 스카이박스 36개 정점 (생략) */
		-1.0f,  1.0f, -1.0f, -1.0f, -1.0f, -1.0f,  1.0f, -1.0f, -1.0f, 1.0f, -1.0f, -1.0f,  1.0f,  1.0f, -1.0f, -1.0f,  1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f, -1.0f, -1.0f, -1.0f, -1.0f,  1.0f, -1.0f, -1.0f,  1.0f, -1.0f, -1.0f,  1.0f,  1.0f, -1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f, -1.0f,  1.0f, -1.0f,  1.0f,  1.0f,  1.0f,  1.0f, 1.0f,  1.0f,  1.0f,  1.0f,  1.0f, -1.0f,  1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f, -1.0f,  1.0f,  1.0f,  1.0f,  1.0f,  1.0f, 1.0f,  1.0f,  1.0f,  1.0f, -1.0f,  1.0f, -1.0f, -1.0f,  1.0f,
		-1.0f,  1.0f, -1.0f,  1.0f,  1.0f, -1.0f,  1.0f,  1.0f,  1.0f, 1.0f,  1.0f,  1.0f,  1.0f,  1.0f, -1.0f, -1.0f,  1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f, -1.0f, -1.0f,  1.0f,  1.0f, -1.0f, -1.0f, 1.0f, -1.0f, -1.0f, -1.0f, -1.0f,  1.0f,  1.0f, -1.0f,  1.0f
	};
	glGenVertexArrays(1, &skyboxVAO); glGenBuffers(1, &skyboxVBO);
	glBindVertexArray(skyboxVAO); glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), skyboxVertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0); glEnableVertexAttribArray(0);
}
// --- 렌더링 최적화 (Greedy Meshing - Row Merging) ---
void BuildRenderList() {
	renderList.clear();

	// 1. 바닥 (Layer 0) - 전체를 덮는 거대한 판 하나
	RenderChunk base;
	base.x = 0.0f; base.z = 0.0f;
	base.w = (float)MAP_SIZE; base.d = (float)MAP_SIZE;
	base.height = 0;
	renderList.push_back(base);

	// 2. 높이가 있는 블록들 병합
	// Z축(세로) 한 줄씩 스캔하며, X축(가로)으로 연속된 같은 높이 블록을 합침
	for (int z = 0; z < MAP_SIZE; ++z) {
		int startX = 0;
		while (startX < MAP_SIZE) {
			int h = mapHeight[startX][z];

			if (h > 0) { // 높이가 0보다 큰 경우만 처리 (0은 이미 base로 그림)
				int endX = startX + 1;
				// 같은 높이가 얼마나 연속되는지 확인
				while (endX < MAP_SIZE && mapHeight[endX][z] == h) {
					endX++;
				}

				// 병합된 덩어리 생성
				float width = (float)(endX - startX);
				float depth = 1.0f; // 한 줄씩 하므로 깊이는 1

				// 월드 좌표 중심 계산
				// 맵의 시작점: -MAP_SIZE/2. 
				// 현재 블록 시작: startX. 길이: width.
				// 중심 X = -MAP_SIZE/2 + startX + width/2
				float worldX = (-MAP_SIZE / 2.0f) + startX + (width / 2.0f);
				float worldZ = (-MAP_SIZE / 2.0f) + z + 0.5f;

				RenderChunk chunk;
				chunk.x = worldX * BLOCK_SIZE;
				chunk.z = worldZ * BLOCK_SIZE;
				chunk.w = width;
				chunk.d = depth;
				chunk.height = h;
				renderList.push_back(chunk);

				startX = endX; // 다음 처리 위치로 점프
			}
			else {
				startX++;
			}
		}
	}
	std::cout << "Map optimized! Render chunks: " << renderList.size() << std::endl;
}
void InitMap() {
	std::srand(std::time(0));
	renderList.clear(); // 렌더링 리스트 초기화

	// 1. 전체 맵을 0(평지)으로 초기화
	for (int i = 0; i < MAP_SIZE; ++i) {
		for (int j = 0; j < MAP_SIZE; ++j) {
			mapHeight[i][j] = 0;
		}
	}

	// 2. 산맥/고원 생성
	int numMountains = 60; // 산의 개수

	for (int m = 0; m < numMountains; ++m) {
		// 위치 랜덤 (이제 중앙을 피할 필요 없음)
		int cx = rand() % MAP_SIZE;
		int cz = rand() % MAP_SIZE;

		// 높이와 넓이 설정
		int maxHeight = 3 + rand() % 6; // 3~8층 높이
		float baseRadius = 40.0f + (rand() % 60); // 넓은 밑변 (40~99)

		// 1층부터 꼭대기까지 쌓아올림 
		for (int h = 1; h <= maxHeight; ++h) {
			// 높이가 올라갈수록 반경이 줄어듦 (계단식)
			float currentRadius = baseRadius * (1.0f - (float)(h - 1) / (float)(maxHeight + 1));

			// 층마다 중심을 살짝 비틀어서 자연스러운 비대칭 모양 형성
			int layerCx = cx + (rand() % 10 - 5);
			int layerCz = cz + (rand() % 10 - 5);





			// 해당 층 채우기
			int range = (int)currentRadius + 2;
			for (int x = layerCx - range; x <= layerCx + range; ++x) {
				for (int z = layerCz - range; z <= layerCz + range; ++z) {
					if (x >= 0 && x < MAP_SIZE && z >= 0 && z < MAP_SIZE) {

						// 중심과의 거리 계산
						float dist = std::sqrt(std::pow(x - layerCx, 2) + std::pow(z - layerCz, 2));

						// [매끄러운 원형] 노이즈 없이 깔끔한 판정
						if (dist < currentRadius) {
							// 기존 높이보다 높을 때만 쌓음 (합집합)
							if (mapHeight[x][z] < h) {
								mapHeight[x][z] = h;
							}
						}
					}
				}
			}
		}
	}

	// 3. 최적화 (Greedy Meshing) 실행
	BuildRenderList();
}
bool IsValidIndex(int x, int z) {
	return (x >= 0 && x < MAP_SIZE && z >= 0 && z < MAP_SIZE);
}
float GetTerrainHeight(float x, float z) {
	int gridX = (int)floor(x / BLOCK_SIZE) + MAP_SIZE / 2;
	int gridZ = (int)floor(z / BLOCK_SIZE) + MAP_SIZE / 2;
	if (IsValidIndex(gridX, gridZ)) {
		return (float)mapHeight[gridX][gridZ] * 1.0f;
	}
	return -100.0f;
}
void Init() {
	glewExperimental = GL_TRUE; glewInit(); glEnable(GL_DEPTH_TEST);
	mainShader = CreateShaderProgram("light_tex_vs.glsl", "light_tex_fs.glsl");
	skyboxShader = CreateShaderProgram("skybox_vs.glsl", "skybox_fs.glsl");
	SetupObjects();
	grassTexture = LoadBitmapTexture("grass.bmp");
	boxTexture = LoadBitmapTexture("box.bmp");
	titleTexture = LoadBitmapTexture("title.bmp");
	failTexture = LoadBitmapTexture("fail.bmp");

	GLuint textureID; glGenTextures(1, &textureID); glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
	GLubyte data[3] = { 135, 206, 235 };
	for (int i = 0; i < 6; i++) glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, 1, 1, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	skyboxTexture = textureID;

	// 맵 생성
	InitMap();

	float centerHeight = GetTerrainHeight(0.0f, 0.0f);

	// 중앙 높이보다 2.0f만큼 위에서 시작 (안전하게 착지하도록)
	//objectPos = glm::vec3(0.0f, centerHeight + 2.0f, 0.0f);
	//cameraTarget = objectPos;

	std::cout << "Spawned at Center Height: " << centerHeight << std::endl;

	//for (int i = 0; i < 256; i++) keyState[i] = false;
}
void ResetGame() {
	// 1. 기존 메모리 해제
	if (myTank) {
		delete myTank;
		myTank = nullptr;
	}
	// 카메라는 Tank 생성자에서 new로 할당되므로, 기존 것은 지워줘야 메모리 누수 방지
	if (myMainCamera) {
		delete myMainCamera;
		myMainCamera = nullptr;
	}
	if (mySubCamera) {
		delete mySubCamera;
		mySubCamera = nullptr;
	}

	// 몬스터 비우기
	for (auto& m : myMonsters) {
		if (m) delete m;
	}
	myMonsters.clear();

	// 총알 비우기
	for (auto& b : myBullets) {
		if (b) delete b;
	}
	myBullets.clear();

	// 2. 객체 재생성 (Init 함수나 main의 로직과 동일하게)
	CreateTank();
	CreateMonster(GetRandomSpawnPos());

	std::cout << "Game Reset Complete!" << std::endl;
}


bool CheckCollision(float targetX, float targetZ, float footY) {
	// 충돌 박스 크기 (플레이어 크기의 절반보다 살짝 작게 설정하여 끼임 방지)
	// playerSize가 0.8이므로 반경은 0.4지만, 0.3 정도로 여유를 둠

	// 검사할 4개의 모서리 좌표
	float corners[4][2] = {
		{ targetX - gTankSize_width / 2.0f * 0.8f, targetZ - gTankSize_depth / 2.0f * 0.8f }, // 왼쪽 위
		{ targetX + gTankSize_width / 2.0f * 0.8f, targetZ - gTankSize_depth / 2.0f * 0.8f }, // 오른쪽 위
		{ targetX - gTankSize_width / 2.0f * 0.8f, targetZ + gTankSize_depth / 2.0f * 0.8f }, // 왼쪽 아래
		{ targetX + gTankSize_width / 2.0f * 0.8f, targetZ + gTankSize_depth / 2.0f * 0.8f }  // 오른쪽 아래
	};

	// 4개 모서리 중 하나라도 높은 벽에 닿으면 충돌로 간주
	for (int i = 0; i < 4; ++i) {
		float h = GetTerrainHeight(corners[i][0], corners[i][1]);
		// 등반 허용 높이 (0.1f) - 이보다 높으면 벽으로 인식
		if (h > footY + 0.1f) {
			return true; // 충돌
		}
	}
	return false; // 안전함
}