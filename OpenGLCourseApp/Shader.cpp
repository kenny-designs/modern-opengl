#include "Shader.h"

Shader::Shader()
{
  shaderID = 0;
  uniformModel = 0;
  uniformProjection = 0;
}

void Shader::CreateFromString(const char* vertexCode, const char* fragmentCode)
{
  CompileShader(vertexCode, fragmentCode);
}

GLuint Shader::GetProjectionLocation()
{
  return uniformProjection;
}

GLuint Shader::GetModelLocation()
{
  return uniformModel;
}

void Shader::UseShader()
{
  glUseProgram(shaderID);
}

void Shader::ClearShader()
{
  if (shaderID != 0)
  {
    glDeleteProgram(shaderID);
    shaderID = 0;
  }

  uniformModel = 0;
  uniformProjection = 0;
}

Shader::~Shader()
{
  ClearShader();
}

void Shader::CompileShader(const char* vertexCode, const char* fragmentCode)
{
  // creates a new shader program and obtains the ID
  // remember! These programs live on the VRAM
  shaderID = glCreateProgram();

  if (!shaderID)
  {
    printf("Error creating shader program!\n");
    return;
  }

  AddShader(shaderID, vertexCode, GL_VERTEX_SHADER);
  AddShader(shaderID, fragmentCode, GL_FRAGMENT_SHADER);

  // tracking errors
  GLint result = 0;
  GLchar eLog[1024] = { 0 };

  // this will create the executables on the graphics card
  glLinkProgram(shaderID);
  // check to see if everything was linked correctly
  glGetProgramiv(shaderID, GL_LINK_STATUS, &result);

  // if result is 0, then something went wrong!
  if (!result)
  {
    // get the error log
    glGetProgramInfoLog(shaderID, sizeof(eLog), NULL, eLog);
    printf("Error linking program: '%s'\n", eLog);
    return;
  }

  // now we check if the shader works with the current context of OpenGL we're
  // working with. To do this, we shall validate it!
  glValidateProgram(shaderID);
  glGetProgramiv(shaderID, GL_VALIDATE_STATUS, &result);

  // if result is 0, then something went wrong!
  if (!result)
  {
    // get the error log
    glGetProgramInfoLog(shaderID, sizeof(eLog), NULL, eLog);
    printf("Error validating program: '%s'\n", eLog);
    return;
  }

  // get uniform IDs
  uniformModel = glGetUniformLocation(shaderID, "model");
  uniformProjection = glGetUniformLocation(shaderID, "projection");

}

void Shader::AddShader(GLuint theProgram, const char* shaderCode, GLenum shaderType)
{
  GLuint theShader = glCreateShader(shaderType);
  const GLchar* theCode[1];
  theCode[0] = shaderCode;

  GLint codeLength[1];
  codeLength[0] = strlen(shaderCode);

  glShaderSource(theShader, 1, theCode, codeLength);
  glCompileShader(theShader);

  // tracking errors
  GLint result = 0;
  GLchar eLog[1024] = { 0 };

  glGetShaderiv(theShader, GL_COMPILE_STATUS, &result);

  // if result is 0, then something went wrong!
  if (!result)
  {
    // get the error log
    glGetShaderInfoLog(theShader, sizeof(eLog), NULL, eLog);
    printf("Error compiling the %d shader: '%s'\n", shaderType, eLog);
    return;
  }

  glAttachShader(theProgram, theShader);
}
