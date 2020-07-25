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

void Camera::keyControl(bool* keys)
{
  if (keys[GLFW_KEY_W])
  {
    position += front * moveSpeed;
  }

  if (keys[GLFW_KEY_S])
  {
    position -= front * moveSpeed;
  }
  
  if (keys[GLFW_KEY_A])
  {
    position -= right * moveSpeed;
  }

  if (keys[GLFW_KEY_D])
  {
    position += right * moveSpeed;
  }
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
