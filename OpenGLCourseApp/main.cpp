#include <stdio.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

// Window dimensions
const GLint WIDTH = 800, HEIGHT = 600;

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

  // Setup viewport size
  glViewport(0, 0, bufferWidth, bufferHeight);

  // loop until window closed
  while (!glfwWindowShouldClose(mainWindow))
  {
    // Get and handle user input events
    glfwPollEvents();

    // Clear window
    glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // There are always two buffers. One that is being drawn to and one that the
    // user can see. Now that we've drawn, do the swap!
    glfwSwapBuffers(mainWindow);
  }

  return 0;
}
