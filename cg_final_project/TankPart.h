#pragma once

#include <gl/glew.h>
#include <gl/freeglut.h>
#include <gl/glm/glm.hpp>

struct Model;

class TankPart
{
public:
	TankPart(Model* model, std::string name);
    ~TankPart();
    void SetColor();
    void SetModelMat(glm::mat4 m);
    void DrawPart(std::string str);
    void SetChild(TankPart* child);
    void ResetModelMat();

private:
    Model* model;
    GLuint VAO;
    GLuint VBO_pos, VBO_nol, EBO;
    GLuint vCount, fCount;
    GLuint uModelLoc, uViewLoc, uProjLoc;
    GLuint uLightPosLoc, uLightColorLoc, uObjColorLoc, uViewPosLoc, uAlphaLoc;
    glm::vec3 uColor;
    float uAlpha;
    glm::mat4 modelMat;
    TankPart* pChild;
    std::string name;
};

