#pragma once

#include <iostream>
#include <vector>
#include <gl/glew.h>
#include <gl/freeglut.h>
#include <gl/glm/glm.hpp>

struct Model;

class Stage
{
public:
    Stage(Model* model, glm::vec3 initLoc);
    ~Stage();
    void DrawStage(std::string str);
    void SetColor();
    glm::vec3 GetCenter();
    float GetSize();

private:
    GLuint VAO;
    GLuint VBO_pos, VBO_nol, EBO;
    GLuint vCount, fCount;
    GLuint uModelLoc, uViewLoc, uProjLoc;
    GLuint uLightPosLoc, uLightColorLoc, uObjColorLoc, uViewPosLoc, uAlphaLoc;
    glm::vec3 uColor;
    float uAlpha;
    glm::mat4 scaleMat;
    glm::mat4 modelMat;
    float size;
    glm::vec3 center;
};

