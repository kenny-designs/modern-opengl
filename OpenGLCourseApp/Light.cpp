#include "Light.h"

Light::Light()
{
  // the idea here is that we're saying how much of a color should be shown
  // so, if red was 0.0f then a red brick would be completely black.
  color = glm::vec3(1.0f, 1.0f, 1.0f);
  ambientIntensity = 1.0f;

  direction = glm::vec3(0.0f, -1.0f, 0.0f);
  diffuseIntensity = 0.0f;
}

Light::Light(GLfloat red, GLfloat green, GLfloat blue, GLfloat aIntensity,
    GLfloat xDir, GLfloat yDir, GLfloat zDir, GLfloat dIntensity)
{
  color = glm::vec3(red, green, blue);
  ambientIntensity = aIntensity;

  direction = glm::vec3(xDir, yDir, zDir);
  diffuseIntensity = dIntensity;
}

void Light::UseLight(GLuint ambientIntensityLocation,
    GLuint ambientColorLocation,
    GLuint diffuseIntensityLocation,
    GLuint directionLocation)
{
  glUniform3f(ambientColorLocation, color.x, color.y, color.z);
  glUniform1f(ambientIntensityLocation, ambientIntensity);

  glUniform3f(directionLocation, direction.x, direction.y, direction.z);
  glUniform1f(diffuseIntensityLocation, diffuseIntensity);
}

Light::~Light(){}
