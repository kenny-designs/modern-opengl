#pragma once

#include <GL/glew.h>

class Material
{
  public:
    Material();
    Material(GLfloat sIntensity, GLfloat shine);
    
    void UseMaterial(GLuint specularIntensityLocation, GLuint shininessLocation);

    ~Material();

  private:
    // dictates how much specular light should be on the object
    GLfloat specularIntensity;
    // how smooth the surface is going to be portrayed as (roughness)
    GLfloat shininess;;
};

