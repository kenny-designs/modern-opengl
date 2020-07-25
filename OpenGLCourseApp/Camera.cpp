#include "Camera.h"

Camera::Camera() {}

Camera::Camera(glm::vec3 startPosition,
    glm::vec3 startUp,
    GLfloat startYaw,
    GLfloat startPitch,
    GLfloat startMoveSpeed,
    GLfloat startTurnSpeed)
{
  position = startPosition;
  worldUp = startUp;
  yaw = startYaw;
  pitch = startPitch;
  front = glm::vec3(0.0f, 0.0f, -1.0f);

  moveSpeed = startMoveSpeed;
  turnSpeed = startTurnSpeed;

  update();
}

void Camera::keyControl(bool* keys, GLfloat deltaTime)
{
  GLfloat velocity = moveSpeed * deltaTime;

  if (keys[GLFW_KEY_W])
  {
    position += front * velocity;
  }

  if (keys[GLFW_KEY_S])
  {
    position -= front * velocity;
  }
  
  if (keys[GLFW_KEY_A])
  {
    position -= right * velocity;
  }

  if (keys[GLFW_KEY_D])
  {
    position += right * velocity;
  }
}

void Camera::mouseControl(GLfloat xChange, GLfloat yChange)
{
  yaw += xChange * turnSpeed;
  pitch += yChange * turnSpeed;

  // limit pitch
  if (pitch > 89.0f)
  {
    pitch = 89.0f;
  }

  if (pitch < -89.0f)
  {
    pitch = -89.0f;
  }

  update();
}

glm::mat4 Camera::calculateViewMatrix()
{
  return glm::lookAt(position, position + front, up);
}

void Camera::update()
{
  // obtain our front vector then normalize
  front.x = cos(glm::radians(pitch)) * cos(glm::radians(yaw));
  front.y = sin(glm::radians(pitch));
  front.z = cos(glm::radians(pitch)) * sin(glm::radians(yaw));
  front = glm::normalize(front);

  // obtain our right vector then normalize it
  right = glm::normalize(glm::cross(front, worldUp));

  // obtain our up vector then normalize it
  up = glm::normalize(glm::cross(right, front));
}

Camera::~Camera() {}
