#pragma once

#include <GL/glew.h>
#include <GL/freeglut.h>

GLvoid DrawScene();
GLvoid Reshape(int w, int h);
GLvoid Keyboard(unsigned char key, int x, int y);
GLvoid KeyboardUp(unsigned char key, int x, int y);
GLvoid SpecialKey(int key, int x, int y);
GLvoid SpecialKeyUp(int key, int x, int y);
GLvoid Mouse(int button, int state, int x, int y);
GLvoid MouseMotion(int x, int y);
GLvoid Timer(int value);