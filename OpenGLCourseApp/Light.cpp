#include "Light.h"

Light::Light()
{
  // the idea here is that we're saying how much of a color should be shown
  // so, if red was 0.0f then a red brick would be completely black.
  color = glm::vec3(1.0f, 1.0f, 1.0f);
  ambientIntensity = 1.0f;
}

Light::Light(GLfloat red, GLfloat green, GLfloat blue, GLfloat aIntensity)
{
  color = glm::vec3(red, green, blue);
  ambientIntensity = aIntensity;
}

void Light::UseLight(GLfloat ambientIntensityLocation, GLfloat ambientColorLocation)
{
  glUniform3f(ambientColorLocation, color.x, color.y, color.z);
  glUniform1f(ambientIntensityLocation, ambientIntensity);
}

Light::~Light(){}
