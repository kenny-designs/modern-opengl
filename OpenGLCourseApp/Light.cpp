#include "Light.h"

Light::Light()
{
  // the idea here is that we're saying how much of a color should be shown
  // so, if red was 0.0f then a red brick would be completely black.
  color = glm::vec3(1.0f, 1.0f, 1.0f);
  ambientIntensity = 1.0f;
  diffuseIntensity = 0.0f;
}

Light::Light(GLfloat red, GLfloat green, GLfloat blue,
    GLfloat aIntensity, GLfloat dIntensity)
{
  color = glm::vec3(red, green, blue);
  ambientIntensity = aIntensity;
  diffuseIntensity = dIntensity;
}

Light::~Light(){}
