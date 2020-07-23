#pragma once

#include <stdio.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

class Window
{
  public:
    Window();
    Window(GLint windowWidth, GLint windowHeight);

    int initialize();

    GLfloat getBufferWidth() { return bufferWidth; }
    GLfloat getBufferHeight() { return bufferHeight; }

    bool getShouldClose() { return glfwWindowShouldClose(mainWindow); }

    void swapBuffers() { glfwSwapBuffers(mainWindow); }

    ~Window();

  private:
    GLFWwindow *mainWindow;
    GLint width, height;
    GLint bufferWidth, bufferHeight;

    bool keys[1024]; // covering the range of ASCII codes

    void createCallbacks();
    static void handleKeys(GLFWwindow* window, int key, int code, int action, int mode);
};
