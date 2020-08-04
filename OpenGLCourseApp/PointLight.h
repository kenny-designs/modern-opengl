#pragma once

#include "Light.h"

class PointLight : public Light
{
  public:
    PointLight();
    PointLight(GLfloat red, GLfloat green, GLfloat blue,
        GLfloat aIntensity, GLfloat dIntensity,
        GLfloat xPos, GLfloat yPos, GLfloat zPos,
        GLfloat con, GLfloat lin, GLfloat exp);

    void UseLight(GLuint ambientIntensityLocation,
        GLuint ambientColorLocation,
        GLuint diffuseIntensityLocation,
        GLuint positionLocation,
        GLuint constantLocation,
        GLuint linearLocation,
        GLuint exponentLocation);

    ~PointLight();

  private:
    glm::vec3 position;

    // controls the attenuation of our light source
    GLfloat constant, linear, exponent;
};
