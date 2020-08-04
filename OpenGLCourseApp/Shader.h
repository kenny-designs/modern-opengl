#pragma once

#include <stdio.h>
#include <string.h>
#include <iostream>
#include <fstream>

#include <GL/glew.h>

#include "DirectionalLight.h"
#include "PointLight.h"

class Shader
{
  public:
    Shader();

    void CreateFromString(const char* vertexCode, const char* fragmentCode);
    void CreateFromFiles(const char* vertexLocation, const char* fragmentLocation);

    std::string ReadFile(const char* fileLocation);

    GLuint GetProjectionLocation();
    GLuint GetModelLocation();
    GLuint GetViewLocation();
    GLuint GetAmbientIntensityLocation();
    GLuint GetColorLocation();
    GLuint GetDiffuseIntensityLocation();
    GLuint GetDirectionLocation();
    GLuint GetSpecularIntensityLocation();
    GLuint GetShininessLocation();
    GLuint GetEyePositionLocation();

    void UseShader();
    void ClearShader();

    ~Shader();

  private:
    int pointLightCount;

    GLuint shaderID,
           uniformProjection,
           uniformModel,
           uniformView,
           uniformEyePosition,
           uniformSpecularIntensity,
           uniformShininess;

    struct {
      GLuint uniformColor;
      GLuint uniformAmbientIntensity;
      GLuint uniformDiffuseIntensity;

      GLuint uniformDirection;
    } uniformDirectionalLight;

    void CompileShader(const char* vertexCode, const char* fragmentCode);
    void AddShader(GLuint theProgram, const char* shaderCode, GLenum shaderType);
};
