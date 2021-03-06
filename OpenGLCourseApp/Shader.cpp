#include "Shader.h"

Shader::Shader()
{
  shaderID = 0;
  uniformModel = 0;
  uniformProjection = 0;

  pointLightCount = 0;
  spotLightCount = 0;
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

GLuint Shader::GetOmniLightPosLocation()
{
  return uniformOmniLightPos;
}

GLuint Shader::GetFarPlaneLocation()
{
  return uniformFarPlane;
}

void Shader::SetDirectionalLight(DirectionalLight* dLight)
{
  dLight->UseLight(uniformDirectionalLight.uniformAmbientIntensity,
      uniformDirectionalLight.uniformColor,
      uniformDirectionalLight.uniformDiffuseIntensity,
      uniformDirectionalLight.uniformDirection);
}

void Shader::SetPointLights(PointLight* pLight,
    unsigned int lightCount,
    unsigned int textureUnit,
    unsigned int offset)
{
  if (lightCount > MAX_POINT_LIGHTS)
  {
    lightCount = MAX_POINT_LIGHTS;
  }

  glUniform1i(uniformPointLightCount, lightCount);

  for (size_t i = 0; i < lightCount; i++)
  {
    pLight[i].UseLight(
        uniformPointLight[i].uniformAmbientIntensity,
        uniformPointLight[i].uniformColor,
        uniformPointLight[i].uniformDiffuseIntensity,
        uniformPointLight[i].uniformPosition,
        uniformPointLight[i].uniformConstant,
        uniformPointLight[i].uniformLinear,
        uniformPointLight[i].uniformExponent);

    pLight[i].GetShadowMap()->Read(GL_TEXTURE0 + textureUnit + i);
    // notice, we don't use GL_TEXTURE0. It doesn't have to be an enum type!
    // also, GL_TEXTURE0 is actually 0x84C0. So, we're not starting from 0 anyways.
    glUniform1i(uniformOmniShadowMap[i + offset].shadowMap, textureUnit + i);
    glUniform1i(uniformOmniShadowMap[i + offset].farPlane, pLight[i].GetFarPlane());
  }
}

void Shader::SetSpotLights(SpotLight* sLight,
    unsigned int lightCount,
    unsigned int textureUnit,
    unsigned int offset)
{
  if (lightCount > MAX_SPOT_LIGHTS)
  {
    lightCount = MAX_SPOT_LIGHTS;
  }

  glUniform1i(uniformSpotLightCount, lightCount);

  for (size_t i = 0; i < lightCount; i++)
  {
    sLight[i].UseLight(
        uniformSpotLight[i].uniformAmbientIntensity,
        uniformSpotLight[i].uniformColor,
        uniformSpotLight[i].uniformDiffuseIntensity,
        uniformSpotLight[i].uniformPosition,
        uniformSpotLight[i].uniformDirection,
        uniformSpotLight[i].uniformConstant,
        uniformSpotLight[i].uniformLinear,
        uniformSpotLight[i].uniformExponent,
        uniformSpotLight[i].uniformEdge);

    sLight[i].GetShadowMap()->Read(GL_TEXTURE0 + textureUnit + i);
    glUniform1i(uniformOmniShadowMap[i + offset].shadowMap, textureUnit + i);
    glUniform1i(uniformOmniShadowMap[i + offset].farPlane, sLight[i].GetFarPlane());
  }
}

void Shader::SetTexture(GLuint textureUnit)
{
  glUniform1i(uniformTexture, textureUnit);
}

void Shader::SetDirectionalShadowMap(GLuint textureUnit)
{
  glUniform1i(uniformDirectionalShadowMap, textureUnit);
}

void Shader::SetDirectionalLightTransform(glm::mat4* lTransform)
{
  glUniformMatrix4fv(
      uniformDirectionalLightTransform,
      1,
      GL_FALSE,
      glm::value_ptr(*lTransform));
}

void Shader::SetLightMatrices(std::vector<glm::mat4> lightMatrices)
{
  for (size_t i = 0; i < 6; i++)
  {
    glUniformMatrix4fv(
        uniformLightMatrices[i],
        1,
        GL_FALSE,
        glm::value_ptr(lightMatrices[i]));
  }
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

void Shader::CreateFromFiles(const char* vertexLocation,
    const char* geometryLocation,
    const char* fragmentLocation)
{
  std::string vertexString = ReadFile(vertexLocation);
  std::string geometryString = ReadFile(geometryLocation);
  std::string fragmentString = ReadFile(fragmentLocation);

  const char* vertexCode = vertexString.c_str();
  const char* geometryCode = geometryString.c_str();
  const char* fragmentCode = fragmentString.c_str();

  CompileShader(vertexCode, geometryCode, fragmentCode);
}

void Shader::Validate()
{
  // tracking errors
  GLint result = 0;
  GLchar eLog[1024] = { 0 };

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

  CompileProgram();
}

void Shader::CompileShader(const char* vertexCode,
    const char* geometryCode,
    const char* fragmentCode)
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
  AddShader(shaderID, geometryCode, GL_GEOMETRY_SHADER);
  AddShader(shaderID, fragmentCode, GL_FRAGMENT_SHADER);

  CompileProgram();
}

void Shader::CompileProgram()
{
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

  // get uniform IDs
  uniformModel = glGetUniformLocation(shaderID, "model");
  uniformProjection = glGetUniformLocation(shaderID, "projection");
  uniformView = glGetUniformLocation(shaderID, "view");
  uniformSpecularIntensity = glGetUniformLocation(shaderID, "material.specularIntensity");
  uniformShininess = glGetUniformLocation(shaderID, "material.shininess");
  uniformEyePosition = glGetUniformLocation(shaderID, "eyePosition");

  uniformDirectionalLight.uniformColor = glGetUniformLocation(shaderID, "directionalLight.base.color");
  uniformDirectionalLight.uniformAmbientIntensity = glGetUniformLocation(shaderID, "directionalLight.base.ambientIntensity");
  uniformDirectionalLight.uniformDirection = glGetUniformLocation(shaderID, "directionalLight.direction");
  uniformDirectionalLight.uniformDiffuseIntensity = glGetUniformLocation(shaderID, "directionalLight.base.diffuseIntensity");

  // Setup all the Point Lights in the scene
  uniformPointLightCount = glGetUniformLocation(shaderID, "pointLightCount");
  for (size_t i = 0; i < MAX_POINT_LIGHTS; i++)
  {
    char locBuff[100] = {'\0'};

    // get uniform location for color
    snprintf(locBuff, sizeof(locBuff), "pointLights[%d].base.color", i);
    uniformPointLight[i].uniformColor = glGetUniformLocation(shaderID, locBuff);

    // get uniform location for ambient intensity
    snprintf(locBuff, sizeof(locBuff), "pointLights[%d].base.ambientIntensity", i);
    uniformPointLight[i].uniformAmbientIntensity = glGetUniformLocation(shaderID, locBuff);

    // get uniform location for diffuse intensity
    snprintf(locBuff, sizeof(locBuff), "pointLights[%d].base.diffuseIntensity", i);
    uniformPointLight[i].uniformDiffuseIntensity = glGetUniformLocation(shaderID, locBuff);

    // get uniform location for position
    snprintf(locBuff, sizeof(locBuff), "pointLights[%d].position", i);
    uniformPointLight[i].uniformPosition = glGetUniformLocation(shaderID, locBuff);

    // get uniform location for constant
    snprintf(locBuff, sizeof(locBuff), "pointLights[%d].constant", i);
    uniformPointLight[i].uniformConstant = glGetUniformLocation(shaderID, locBuff);

    // get uniform location for linear
    snprintf(locBuff, sizeof(locBuff), "pointLights[%d].linear", i);
    uniformPointLight[i].uniformLinear = glGetUniformLocation(shaderID, locBuff);

    // get uniform location for exponent
    snprintf(locBuff, sizeof(locBuff), "pointLights[%d].exponent", i);
    uniformPointLight[i].uniformExponent = glGetUniformLocation(shaderID, locBuff);
  }


  // Setup all the Spot Lights in the scene
  uniformSpotLightCount = glGetUniformLocation(shaderID, "spotLightCount");
  for (size_t i = 0; i < MAX_SPOT_LIGHTS; i++)
  {
    char locBuff[100] = {'\0'};

    // get uniform location for color
    snprintf(locBuff, sizeof(locBuff), "spotLights[%d].base.base.color", i);
    uniformSpotLight[i].uniformColor = glGetUniformLocation(shaderID, locBuff);

    // get uniform location for ambient intensity
    snprintf(locBuff, sizeof(locBuff), "spotLights[%d].base.base.ambientIntensity", i);
    uniformSpotLight[i].uniformAmbientIntensity = glGetUniformLocation(shaderID, locBuff);

    // get uniform location for diffuse intensity
    snprintf(locBuff, sizeof(locBuff), "spotLights[%d].base.base.diffuseIntensity", i);
    uniformSpotLight[i].uniformDiffuseIntensity = glGetUniformLocation(shaderID, locBuff);

    // get uniform location for position
    snprintf(locBuff, sizeof(locBuff), "spotLights[%d].base.position", i);
    uniformSpotLight[i].uniformPosition = glGetUniformLocation(shaderID, locBuff);

    // get uniform location for constant
    snprintf(locBuff, sizeof(locBuff), "spotLights[%d].base.constant", i);
    uniformSpotLight[i].uniformConstant = glGetUniformLocation(shaderID, locBuff);

    // get uniform location for linear
    snprintf(locBuff, sizeof(locBuff), "spotLights[%d].base.linear", i);
    uniformSpotLight[i].uniformLinear = glGetUniformLocation(shaderID, locBuff);

    // get uniform location for exponent
    snprintf(locBuff, sizeof(locBuff), "spotLights[%d].base.exponent", i);
    uniformSpotLight[i].uniformExponent = glGetUniformLocation(shaderID, locBuff);

    // get uniform location for direction
    snprintf(locBuff, sizeof(locBuff), "spotLights[%d].direction", i);
    uniformSpotLight[i].uniformDirection = glGetUniformLocation(shaderID, locBuff);

    // get uniform location for edge
    snprintf(locBuff, sizeof(locBuff), "spotLights[%d].edge", i);
    uniformSpotLight[i].uniformEdge = glGetUniformLocation(shaderID, locBuff);
  }

  // Bind uniforms for textures
  uniformTexture = glGetUniformLocation(shaderID, "theTexture");
  uniformDirectionalLightTransform = glGetUniformLocation(shaderID, "directionalLightTransform");
  uniformDirectionalShadowMap = glGetUniformLocation(shaderID, "directionalShadowMap");

  // Bind uniforms for omni-light shadows
  uniformOmniLightPos = glGetUniformLocation(shaderID, "lightPos");
  uniformFarPlane = glGetUniformLocation(shaderID, "farPlane");

  // Now for each light matrix for our cubemap
  for (size_t i = 0; i < 6; i++)
  {
    char locBuff[100] = { '\0' };

    snprintf(locBuff, sizeof(locBuff), "lightMatrices[%d]", i);
    uniformLightMatrices[i] = glGetUniformLocation(shaderID, locBuff);
  }

  // Get the uniforms for the omni shadows
  for (size_t i = 0; i < MAX_POINT_LIGHTS + MAX_SPOT_LIGHTS; i++)
  {
    char locBuff[100] = { '\0' };

    snprintf(locBuff, sizeof(locBuff), "omniShadowMaps[%d].shadowMap", i);
    uniformOmniShadowMap[i].shadowMap = glGetUniformLocation(shaderID, locBuff);

    snprintf(locBuff, sizeof(locBuff), "omniShadowMaps[%d].farPlane", i);
    uniformOmniShadowMap[i].farPlane = glGetUniformLocation(shaderID, locBuff);
  }
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
