#include "Window.h"

Window::Window()
{
  width = 800;
  height = 600;
  mouseFirstMoved = true;

  for (size_t i = 0; i < 1024; i++)
  {
    keys[i] = 0;
  }
}

Window::Window(GLint windowWidth, GLint windowHeight)
{
  width = windowWidth;
  height = windowHeight;
  mouseFirstMoved = true;

  for (size_t i = 0; i < 1024; i++)
  {
    keys[i] = 0;
  }
}

int Window::initialize()
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
  mainWindow = glfwCreateWindow(width, height, "Test Window", NULL, NULL);
  if (!mainWindow)
  {
    printf("GLFW window creation failed!");
    glfwTerminate();
    return 1;
  }

  // Get buffer size information
  glfwGetFramebufferSize(mainWindow, &bufferWidth, &bufferHeight);

  // Set context for GLEW to use
  glfwMakeContextCurrent(mainWindow);

  // Handle Key + Mouse Input
  createCallbacks();
  // Hide the cursor like in most games
  glfwSetInputMode(mainWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

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

  // we pass in the main window and the 'owner' of it (which is this class)
  // this aids in callbacks for user input
  glfwSetWindowUserPointer(mainWindow, this);

  return 0;
}

void Window::createCallbacks()
{
  glfwSetKeyCallback(mainWindow, handleKeys);
  glfwSetCursorPosCallback(mainWindow, handleMouse);
}

GLfloat Window::getXChange()
{
  GLfloat theChange = xChange;
  xChange = 0.0f;
  return theChange;
}

GLfloat Window::getYChange()
{
  GLfloat theChange = yChange;
  yChange = 0.0f;
  return theChange;
}


void Window::handleKeys(GLFWwindow* window, int key, int code, int action, int mode)
{
  Window* theWindow = static_cast<Window*>(glfwGetWindowUserPointer(window));

  if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
  {
    glfwSetWindowShouldClose(window, GL_TRUE);
  }

  if (key >= 0 && key < 1024)
  {
    if (action == GLFW_PRESS)
    {
      theWindow->keys[key] = true;
    }
    else if (action == GLFW_RELEASE)
    {
      theWindow->keys[key] = false;
    }
  }
}

void Window::handleMouse(GLFWwindow* window, double xPos, double yPos)
{
  Window* theWindow = static_cast<Window*>(glfwGetWindowUserPointer(window));

  if (theWindow->mouseFirstMoved)
  {
    theWindow->lastX = xPos;
    theWindow->lastY = yPos;
    theWindow->mouseFirstMoved = false;
  }

  theWindow->xChange = xPos - theWindow->lastX;
  // we do this to avoid inverted up and down movement
  theWindow->yChange = theWindow->lastY - yPos; 

  theWindow->lastX = xPos;
  theWindow->lastY = yPos;
}

Window::~Window()
{
  glfwDestroyWindow(mainWindow);
  glfwTerminate();
}

