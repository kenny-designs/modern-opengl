#include <stdio.h>
#include <string.h>
#include <cmath> // abs()

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Window dimensions
const GLint WIDTH = 800, HEIGHT = 600;
const float toRadians = 3.14159265f / 180.0f;

// Track the IDs
GLuint VAO, VBO, IBO, shader, uniformModel, uniformProjection;

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


void CreateTriangle()
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

  // adds one vertex array to the VRAM and obtain the ID for it
  glGenVertexArrays(1, &VAO);
  glBindVertexArray(VAO);

  // setup the IBO
  // The IBO and VBO are connected through the VAO by the way!
  glGenBuffers(1, &IBO);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO); // element and index are interchangable here
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

  // similar to the VAO, we create a single buffer and obtain the ID for it
  glGenBuffers(1, &VBO);
  // there are various targets we can bind the VBO to. Here, we'll bind to the
  // GL_ARRAY_BUFFER 
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  // GL_STATIC_DRAW means that we don't plan on changing the values within the
  // vertices array. If you do intend to, then use GL_DYNAMIC_DRAW.
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  // a good little bit happening here. Refer to the documentation
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(0);

  // Unbind the VAO, VBO, and IBO
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void AddShader(GLuint theProgram, const char* shaderCode, GLenum shaderType)
{
  GLuint theShader = glCreateShader(shaderType);
  const GLchar* theCode[1];
  theCode[0] = shaderCode;

  GLint codeLength[1];
  codeLength[0] = strlen(shaderCode);

  glShaderSource(theShader, 1, theCode, codeLength);
  glCompileShader(theShader);

  // tracking errors
  GLint result = 0;
  GLchar eLog[1024] = { 0 };

  glGetShaderiv(theShader, GL_COMPILE_STATUS, &result);

  // if result is 0, then something went wrong!
  if (!result)
  {
    // get the error log
    glGetShaderInfoLog(theShader, sizeof(eLog), NULL, eLog);
    printf("Error compiling the %d shader: '%s'\n", shaderType, eLog);
    return;
  }

  glAttachShader(theProgram, theShader);
}

void CompileShader()
{
  // creates a new shader program and obtains the ID
  // remember! These programs live on the VRAM
  shader = glCreateProgram();

  if (!shader)
  {
    printf("Error creating shader program!\n");
    return;
  }

  AddShader(shader, vShader, GL_VERTEX_SHADER);
  AddShader(shader, fShader, GL_FRAGMENT_SHADER);

  // tracking errors
  GLint result = 0;
  GLchar eLog[1024] = { 0 };

  // this will create the executables on the graphics card
  glLinkProgram(shader);
  // check to see if everything was linked correctly
  glGetProgramiv(shader, GL_LINK_STATUS, &result);

  // if result is 0, then something went wrong!
  if (!result)
  {
    // get the error log
    glGetProgramInfoLog(shader, sizeof(eLog), NULL, eLog);
    printf("Error linking program: '%s'\n", eLog);
    return;
  }

  // now we check if the shader works with the current context of OpenGL we're
  // working with. To do this, we shall validate it!
  glValidateProgram(shader);
  glGetProgramiv(shader, GL_VALIDATE_STATUS, &result);

  // if result is 0, then something went wrong!
  if (!result)
  {
    // get the error log
    glGetProgramInfoLog(shader, sizeof(eLog), NULL, eLog);
    printf("Error validating program: '%s'\n", eLog);
    return;
  }

  // get uniform IDs
  uniformModel = glGetUniformLocation(shader, "model");
  uniformProjection = glGetUniformLocation(shader, "projection");
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

  CreateTriangle();
  CompileShader();

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
    glUseProgram(shader);

    // create an identity matrix
    glm::mat4 model(1.0f);

    // translate the triangle by the offset
    model = glm::translate(model, glm::vec3(0.0f, 0.0f, -2.5f));

    // rotate 45 degrees on the z-axis
    model = glm::rotate(model, curRot * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));

    // scale the model
    model = glm::scale(model, glm::vec3(0.4f, 0.4f, 1.0f));

    // apply the transformations
    glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));

    // bind our VAO
    glBindVertexArray(VAO);

    // bind our IBO
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);

    // draw
    glDrawElements(GL_TRIANGLES, 12, GL_UNSIGNED_INT, 0);

    // unbind our IBO
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    // unbind our VAO
    glBindVertexArray(0);

    // remove our shader program
    glUseProgram(0);

    // There are always two buffers. One that is being drawn to and one that the
    // user can see. Now that we've drawn, do the swap!
    glfwSwapBuffers(mainWindow);
  }

  return 0;
}
