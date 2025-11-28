#pragma once

#include <gl/glew.h>
#include <gl/freeglut.h>

struct Model;
struct Vertex;
struct Face;

class Monster;

void read_newline(char* str);
void read_obj_file(const char* filename, Model* model);
void print_model_info(const Model* model);
void InitBuffers(GLuint& VAO, GLuint& VBO_pos, GLuint& VBO_nol, GLuint& EBO,
    Vertex* vData, Vertex* vNormal, Face* fData, size_t vCount, size_t fCount, size_t nCount);

char* filetobuf(const char* file);
void make_vertexShaders();
void make_fragmentShaders();
GLuint make_shaderProgram();

void SetProjMatMain(); // set projection matrix for main screen
void SetProjMatSub(); // set projection matrix for mini map

void CreatePlane();
void CreateTank();
void CreateMonster(glm::vec3 initLoc);




GLuint LoadBitmapTexture(const char* imagepath);
GLuint CreateColorCubemap(int r, int g, int b);
std::string LoadShaderSource(const char* filename);
GLuint CreateShaderProgram(const char* vsFile, const char* fsFile);
void SetupObjects();
void BuildRenderList();
void InitMap();
bool IsValidIndex(int x, int z);
float GetTerrainHeight(float x, float z);
void Init();
bool CheckCollision(float targetX, float targetZ, float footY);

template<typename T>
void RemoveDestroyed(std::vector<T*>& myVec) {
    for (auto it = myVec.begin(); it != myVec.end();) {
        if ((*it)->GetDestroyed()) {
            delete* it;
            it = myVec.erase(it);
        }
        else {
            it++;
        }
    }
}