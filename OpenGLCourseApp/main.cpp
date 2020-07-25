#include <stdio.h>
#include <string.h>
#include <cmath> // abs()
#include <vector>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Window.h"
#include "Mesh.h"
#include "Shader.h"
#include "Camera.h"

// Window dimensions
const float toRadians = 3.14159265f / 180.0f;

Window mainWindow;
std::vector<Mesh*> meshList;
std::vector<Shader> shaderList;
Camera camera;

GLfloat deltaTime = 0.0f;
GLfloat lastTime = 0.0f;

// Vertex Shader
static const char* vShader = "Shaders/shader.vert";

// Fragment Shader
static const char* fShader = "Shaders/shader.frag";


void CreateObjects()
{
  // The indices that make up our pyramid
  unsigned int indices[] = {
    0, 3, 1,
    1, 3, 2,
    2, 3, 0,
    0, 1, 2
  };

  // define the vertices for the triangle
  GLfloat vertices[] = {
    -1.0f, -1.0f, 0.0f,
     0.0f, -1.0f, 1.0f,
     1.0f, -1.0f, 0.0f,
     0.0f,  1.0f, 0.0f
  };

  Mesh *obj1 = new Mesh();
  obj1->CreateMesh(vertices, indices, 12, 12);
  meshList.push_back(obj1);

  Mesh *obj2 = new Mesh();
  obj2->CreateMesh(vertices, indices, 12, 12);
  meshList.push_back(obj2);
}

void CreateShaders()
{
  Shader *shader1 = new Shader();
  shader1->CreateFromFiles(vShader, fShader);
  shaderList.push_back(*shader1);
}

int main()
{
  mainWindow = Window(800, 600);
  mainWindow.initialize();

  CreateObjects();
  CreateShaders();

  camera = Camera(glm::vec3(0.0f, 0.0f, 0.0f),
      glm::vec3(0.0f, 1.0f, 0.0f),
      -90.0f,
      0.0f,
      5.0f,
      0.5f);

  GLuint uniformProjection = 0,
         uniformModel = 0,
         uniformView = 0;

  // Prepare the projection matrix
  glm::mat4 projection = glm::perspective(
      45.0f,
      mainWindow.getBufferWidth() / mainWindow.getBufferHeight(), 
      0.1f,
      100.0f);

  // loop until window closed
  while (!mainWindow.getShouldClose())
  {
    GLfloat now = glfwGetTime();
    deltaTime = now - lastTime;
    lastTime = now;

    // Get and handle user input events
    glfwPollEvents();

    // User input for the camera
    camera.keyControl(mainWindow.getKeys(), deltaTime);
    camera.mouseControl(mainWindow.getXChange(), mainWindow.getYChange());

    // Clear window
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // use our shader program
    shaderList[0].UseShader();
    uniformModel = shaderList[0].GetModelLocation();
    uniformProjection = shaderList[0].GetProjectionLocation();
    uniformView = shaderList[0].GetViewLocation();

    // create an identity matrix
    glm::mat4 model(1.0f);

    model = glm::translate(model, glm::vec3(0.0f, 0.0f, -2.5f));
    model = glm::rotate(model, 0.0f, glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::scale(model, glm::vec3(0.4f, 0.4f, 1.0f));

    // apply the transformations
    glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
    glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));

    meshList[0]->RenderMesh();

    model = glm::mat4(1.0f);

    model = glm::translate(model, glm::vec3(0.0f, 1.0f, -2.5f));
    model = glm::rotate(model, 0.0f, glm::vec3(0.0f, -1.0f, 0.0f));
    model = glm::scale(model, glm::vec3(0.4f, 0.4f, 1.0f));
    glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));

    meshList[1]->RenderMesh();

    // remove our shader program
    glUseProgram(0);

    mainWindow.swapBuffers();
  }

  return 0;
}
