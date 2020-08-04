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

GLuint Shader::GetViewLocation()
{
  return uniformView;
}

GLuint Shader::GetAmbientIntensityLocation()
{
  return uniformDirectionalLight.uniformAmbientIntensity;
}

GLuint Shader::GetColorLocation()
{
  return uniformDirectionalLight.uniformColor;
}

GLuint Shader::GetDiffuseIntensityLocation()
{
  return uniformDirectionalLight.uniformDiffuseIntensity;
}

GLuint Shader::GetDirectionLocation()
{
  return uniformDirectionalLight.uniformDirection;
}

GLuint Shader::GetSpecularIntensityLocation()
{
  return uniformSpecularIntensity;
}

GLuint Shader::GetShininessLocation()
{
  return uniformShininess;
}

GLuint Shader::GetEyePositionLocation()
{
  return uniformEyePosition;
}

void Shader::SetDirectionalLight(DirectionalLight* dLight)
{
  dLight->UseLight(uniformDirectionalLight.uniformAmbientIntensity,
      uniformDirectionalLight.uniformColor,
      uniformDirectionalLight.uniformDiffuseIntensity,
      uniformDirectionalLight.uniformDirection);
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


void Shader::CreateFromFiles(const char* vertexLocation,
    const char* fragmentLocation)
{
  std::string vertexString = ReadFile(vertexLocation);
  std::string fragmentString = ReadFile(fragmentLocation);

  const char* vertexCode = vertexString.c_str();
  const char* fragmentCode = fragmentString.c_str();

  CompileShader(vertexCode, fragmentCode);
}

std::string Shader::ReadFile(const char* fileLocation)
{
  std::string content;
  std::ifstream fileStream(fileLocation, std::ios::in);

  if (!fileStream.is_open())
  {
    printf("Failed to read %s! File doesn't exist\n", fileLocation);
    return "";
  }

  std::string line = "";
  while (!fileStream.eof())
  {
    std::getline(fileStream, line);
    content.append(line + "\n");
  }

  fileStream.close();
  return content;
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
  uniformView = glGetUniformLocation(shaderID, "view");
  uniformSpecularIntensity = glGetUniformLocation(shaderID, "material.specularIntensity");
  uniformShininess = glGetUniformLocation(shaderID, "material.shininess");
  uniformEyePosition = glGetUniformLocation(shaderID, "eyePosition");

  uniformDirectionalLight.uniformColor = glGetUniformLocation(shaderID, "directionalLight.color");
  uniformDirectionalLight.uniformAmbientIntensity = glGetUniformLocation(shaderID, "directionalLight.ambientIntensity");
  uniformDirectionalLight.uniformDirection = glGetUniformLocation(shaderID, "directionalLight.direction");
  uniformDirectionalLight.uniformDiffuseIntensity = glGetUniformLocation(shaderID, "directionalLight.diffuseIntensity");
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
