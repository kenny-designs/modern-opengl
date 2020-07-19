#include <stdio.h>
#include <string.h>
#include <cmath> // abs()
#include <vector>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Mesh.h"
#include "Shader.h"

// Window dimensions
const GLint WIDTH = 800, HEIGHT = 600;
const float toRadians = 3.14159265f / 180.0f;

std::vector<Mesh*> meshList;
std::vector<Shader> shaderList;

// Controlling triangle movement
bool direction = true;
float triOffset = 0.0f;
float triMaxoffset = 0.7f;
float triIncrement = 0.005f;

// Controlling triangle rotation
float curRot = 0.0f;
float rotIncrement = 0.5f;

// Vertex Shader
static const char* vShader = "                          \n\
#version 330                                            \n\
                                                        \n\
layout (location = 0) in vec3 pos;                      \n\
                                                        \n\
out vec4 vCol;                                          \n\
                                                        \n\
uniform mat4 model;                                     \n\
uniform mat4 projection;                                \n\
                                                        \n\
void main()                                             \n\
{                                                       \n\
  gl_Position = projection * model * vec4(pos, 1.0f);   \n\
  vCol = vec4(clamp(pos, 0.0f, 1.0f), 1.0f);            \n\
}";

// Fragment Shader
static const char* fShader = "                    \n\
#version 330                                      \n\
                                                  \n\
in vec4 vCol;                                     \n\
                                                  \n\
out vec4 color;                                   \n\
                                                  \n\
void main()                                       \n\
{                                                 \n\
  color = vCol;                                   \n\
}";


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
  shader1->CreateFromString(vShader, fShader);
  shaderList.push_back(*shader1);
}

int main()
{
  // Initialize GLFW
  if (!glfwInit())
  {
    printf("GLFW Initialization failed!");
    glfwTerminate();
    return 1;
  }

  // Setup GLFW window properties
  // OpenGL version 3.3
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  
  // Don't use deprecated features from previous versions
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  // We do want it to be forwards compatible though
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

  // Now create the window itself
  GLFWwindow *mainWindow = glfwCreateWindow(WIDTH, HEIGHT, "Test Window", NULL, NULL);
  if (!mainWindow)
  {
    printf("GLFW window creation failed!");
    glfwTerminate();
    return 1;
  }

  // Get buffer size information
  int bufferWidth, bufferHeight;
  glfwGetFramebufferSize(mainWindow, &bufferWidth, &bufferHeight);

  // Set context for GLEW to use
  glfwMakeContextCurrent(mainWindow);

  // Allow modern extension features
  glewExperimental = GL_TRUE;

  // Attempt to initialize GLEW
  if (glewInit() != GLEW_OK)
  {
    printf("GLEW initialization failed!");
    glfwDestroyWindow(mainWindow);
    glfwTerminate();
    return 1;
  }

  // enable depth testing
  glEnable(GL_DEPTH_TEST);

  // Setup viewport size
  glViewport(0, 0, bufferWidth, bufferHeight);

  CreateObjects();
  CreateShaders();

  GLuint uniformProjection = 0,
         uniformModel = 0;

  // Prepare the projection matrix
  glm::mat4 projection = glm::perspective(
      45.0f,
      (GLfloat)bufferWidth / (GLfloat)bufferHeight,
      0.1f,
      100.0f);

  // loop until window closed
  while (!glfwWindowShouldClose(mainWindow))
  {
    // Get and handle user input events
    glfwPollEvents();

    // move the triangle
    if (direction)
    {
      triOffset += triIncrement;
    }
    else
    { 
      triOffset -= triIncrement;
    }

    // bounce the triangle off the sides of the window
    if (abs(triOffset) >= triMaxoffset)
    {
      direction = !direction;
    }

    // rotate the triangle
    curRot += rotIncrement;
    if (curRot >= 360.0f)
    {
      curRot -= 360.0f;
    }

    // Clear window
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // use our shader program
    shaderList[0].UseShader();
    uniformModel = shaderList[0].GetModelLocation();
    uniformProjection = shaderList[0].GetProjectionLocation();

    // create an identity matrix
    glm::mat4 model(1.0f);

    // translate the triangle by the offset
    model = glm::translate(model, glm::vec3(0.0f, 0.0f, -2.5f));

    // rotate 45 degrees on the z-axis
    //
    // void UseShader();
    // void ClearShader();
    model = glm::rotate(model, curRot * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));

    // scale the model
    model = glm::scale(model, glm::vec3(0.4f, 0.4f, 1.0f));

    // apply the transformations
    glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));

    meshList[0]->RenderMesh();

    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, 1.0f, -2.5f));
    model = glm::rotate(model, curRot * toRadians, glm::vec3(0.0f, -1.0f, 0.0f));
    model = glm::scale(model, glm::vec3(0.4f, 0.4f, 1.0f));
    glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));

    meshList[1]->RenderMesh();

    // remove our shader program
    glUseProgram(0);

    // There are always two buffers. One that is being drawn to and one that the
    // user can see. Now that we've drawn, do the swap!
    glfwSwapBuffers(mainWindow);
  }

  return 0;
}
