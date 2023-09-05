#pragma once

/*请确认是在包含GLFW的头文件之前包含了GLAD的头文件。
GLAD的头文件包含了正确的OpenGL头文件（例如GL/gl.h），所以需要在其它依赖于OpenGL的头文件之前包含GLAD。*/
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>

int Triangle();