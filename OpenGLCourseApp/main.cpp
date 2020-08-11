#define STB_IMAGE_IMPLEMENTATION

#include <stdio.h>
#include <string.h>
#include <cmath> // abs()
#include <vector>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "CommonValues.h"

#include "Window.h"
#include "Mesh.h"
#include "Shader.h"
#include "Camera.h"
#include "Texture.h"
#include "DirectionalLight.h"
#include "PointLight.h"
#include "SpotLight.h"
#include "Material.h"

#include "Model.h"

// Window dimensions
const float toRadians = 3.14159265f / 180.0f;

// Uniform globals used for rendering
GLuint uniformProjection = 0,
       uniformModel = 0,
       uniformView = 0,
       uniformEyePosition = 0,
       uniformSpecularIntensity = 0,
       uniformShininess = 0,
       uniformOmniLightPos = 0,
       uniformFarPlane = 0;

Window mainWindow;

std::vector<Mesh*> meshList;

std::vector<Shader> shaderList;
Shader directionalShadowShader;
Shader omniShadowShader;

Camera camera;

Texture brickTexture;
Texture dirtTexture;
Texture plainTexture;

Material shinyMaterial;
Material dullMaterial;

Model xwing;
Model blackhawk;

DirectionalLight mainLight;
PointLight pointLights[MAX_POINT_LIGHTS];
SpotLight spotLights[MAX_SPOT_LIGHTS];

unsigned int pointLightCount = 0;
unsigned int spotLightCount = 0;

GLfloat deltaTime = 0.0f;
GLfloat lastTime = 0.0f;

GLfloat blackhawkAngle = 0.0f;

// Vertex Shader
static const char* vShader = "Shaders/shader.vert";

// Fragment Shader
static const char* fShader = "Shaders/shader.frag";

void calcAverageNormals(unsigned int * indices,
    unsigned int indiceCount,
    GLfloat * vertices,
    unsigned int verticeCount,
    unsigned int vLength,
    unsigned int normalOffset)
{
  for (size_t i = 0; i < indiceCount; i += 3)
  {
    unsigned int in0 = indices[i] * vLength;
    unsigned int in1 = indices[i + 1] * vLength;
    unsigned int in2 = indices[i + 2] * vLength;

    // similar to what I did in calc3,
    // create a plane and then cross product
    // to get the normal
    glm::vec3 v1(vertices[in1] - vertices[in0],
        vertices[in1 + 1] - vertices[in0 + 1],
        vertices[in1 + 2] - vertices[in0 + 2]);

    glm::vec3 v2(vertices[in2] - vertices[in0],
        vertices[in2 + 1] - vertices[in0 + 1],
        vertices[in2 + 2] - vertices[in0 + 2]);

    // now we have our normal! Don't forget to normalize it.
    glm::vec3 normal = glm::cross(v1, v2);
    normal = glm::normalize(normal);

    // now we can offset our normals in the vertices
    // since we already have the x, y, and z we'll just offset
    in0 += normalOffset;
    in1 += normalOffset;
    in2 += normalOffset;

    // adjust the first normal's vertices
    vertices[in0] += normal.x;
    vertices[in0 + 1] += normal.y;
    vertices[in0 + 2] += normal.z;

    // now the second
    vertices[in1] += normal.x;
    vertices[in1 + 1] += normal.y;
    vertices[in1 + 2] += normal.z;

    // and now the final third vertice
    vertices[in2] += normal.x;
    vertices[in2 + 1] += normal.y;
    vertices[in2 + 2] += normal.z;
  }

  for (size_t i = 0; i < verticeCount / vLength; i++)
  {
    unsigned int nOffset = i * vLength + normalOffset;
    glm::vec3 vec(vertices[nOffset], vertices[nOffset + 1], vertices[nOffset + 2]);
    vec = glm::normalize(vec);

    vertices[nOffset] = vec.x;
    vertices[nOffset + 1] = vec.y;
    vertices[nOffset + 2] = vec.z;
  }
}

void CreateObjects()
{
  // The indices that make up our pyramid
  unsigned int indices[] = {
    0, 3, 1,
    1, 3, 2,
    2, 3, 0,
    0, 1, 2
  };

  // define the vertices for the triangle
  GLfloat vertices[] = {
    // nx, ny, and nz is for the normals
    // x,     y,    z,      u,    v     nx    ny    nz
    -1.0f, -1.0f, -0.6f,   0.0f, 0.0f,   0.0f, 0.0f, 0.0f,
    0.0f, -1.0f, 1.0f,   0.5f, 0.0f,   0.0f, 0.0f, 0.0f,
    1.0f, -1.0f, -0.6f,   1.0f, 0.0f,   0.0f, 0.0f, 0.0f,
    0.0f,  1.0f, 0.0f,   0.5f, 1.0f,   0.0f, 0.0f, 0.0f
  };

  unsigned int floorIndices[] = {
    0, 2, 1,
    1, 2, 3
  };

  GLfloat floorVertices[] = {
    -10.0f, 0.0f, -10.0f,   0.0f, 0.0f,   0.0f, -1.0f, 0.0f,
    10.0f, 0.0f, -10.0f,    10.0f, 0.0f,  0.0f, -1.0f, 0.0f,
    -10.0f, 0.0f, 10.0f,    0.0f, 10.0f,  0.0f, -1.0f, 0.0f,
    10.0f, 0.0f, 10.0f,     10.0f, 10.0f, 0.0f, -1.0f, 0.0f
  };

  calcAverageNormals(indices, 12, vertices, 32, 8, 5);

  Mesh *obj1 = new Mesh();
  obj1->CreateMesh(vertices, indices, 32, 12);
  meshList.push_back(obj1);

  Mesh *obj2 = new Mesh();
  obj2->CreateMesh(vertices, indices, 32, 12);
  meshList.push_back(obj2);

  Mesh *obj3 = new Mesh();
  obj3->CreateMesh(floorVertices, floorIndices, 32, 6);
  meshList.push_back(obj3);
}

void CreateShaders()
{
  Shader *shader1 = new Shader();
  shader1->CreateFromFiles(vShader, fShader);
  shaderList.push_back(*shader1);

  directionalShadowShader = Shader();
  directionalShadowShader.CreateFromFiles(
      "Shaders/directional_shadow_map.vert",
      "Shaders/directional_shadow_map.frag");

  omniShadowShader = Shader();
  omniShadowShader.CreateFromFiles(
      "Shaders/omni_shadow_map.vert",
      "Shaders/omni_shadow_map.geom",
      "Shaders/omni_shadow_map.frag");
}

void RenderScene()
{
  // Position and draw the first mesh
  glm::mat4 model(1.0f);
  model = glm::translate(model, glm::vec3(0.0f, 0.0f, -2.5f));
  model = glm::rotate(model, 0.0f, glm::vec3(0.0f, 1.0f, 0.0f));
  glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
  brickTexture.UseTexture();
  shinyMaterial.UseMaterial(uniformSpecularIntensity, uniformShininess);
  meshList[0]->RenderMesh();

  // Position and draw the second mesh
  model = glm::mat4(1.0f);
  model = glm::translate(model, glm::vec3(0.0f, 4.0f, -2.5f));
  model = glm::rotate(model, 0.0f, glm::vec3(0.0f, -1.0f, 0.0f));
  glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
  dirtTexture.UseTexture();
  dullMaterial.UseMaterial(uniformSpecularIntensity, uniformShininess);
  meshList[1]->RenderMesh();

  // Position and draw the third mesh
  model = glm::mat4(1.0f);
  model = glm::translate(model, glm::vec3(0.0f, -2.0f, 0.0f));
  glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
  dirtTexture.UseTexture();
  shinyMaterial.UseMaterial(uniformSpecularIntensity, uniformShininess);
  meshList[2]->RenderMesh();

  // Render the xwing model
  model = glm::mat4(1.0f);
  model = glm::translate(model, glm::vec3(-7.0f, 0.0f, 10.0f));
  model = glm::scale(model, glm::vec3(0.006, 0.006f, 0.006f));
  glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
  shinyMaterial.UseMaterial(uniformSpecularIntensity, uniformShininess);
  xwing.RenderModel();

  // move the blackhawk model
  blackhawkAngle += 0.1f;
  if (blackhawkAngle > 360.0f)
  {
    blackhawkAngle = 0.1f;
  }

  // Render the blackhawk model
  model = glm::mat4(1.0f);
  model = glm::rotate(model, -blackhawkAngle * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
  model = glm::translate(model, glm::vec3(-8.0f, 2.0f, 0.0f));
  model = glm::rotate(model, -20.0f * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
  model = glm::rotate(model, -90.0f * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
  model = glm::scale(model, glm::vec3(0.4, 0.4f, 0.4f));
  glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
  shinyMaterial.UseMaterial(uniformSpecularIntensity, uniformShininess);
  blackhawk.RenderModel();
}

void DirectionalShadowMapPass(DirectionalLight* light)
{
  directionalShadowShader.UseShader();
  glViewport(0, 0,
      light->GetShadowMap()->GetShadowWidth(),
      light->GetShadowMap()->GetShadowHeight());

  light->GetShadowMap()->Write();
  glClear(GL_DEPTH_BUFFER_BIT);

  uniformModel = directionalShadowShader.GetModelLocation();
  //directionalShadowShader.SetDirectionalLightTransform(&light->CalculateLightTransform());
  glm::mat4 foo = light->CalculateLightTransform();
  directionalShadowShader.SetDirectionalLightTransform(&foo);

  RenderScene();

  // unbind
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void OmniShadowMapPass(PointLight* light)
{
  omniShadowShader.UseShader();
  glViewport(0, 0,
      light->GetShadowMap()->GetShadowWidth(),
      light->GetShadowMap()->GetShadowHeight());

  light->GetShadowMap()->Write();
  glClear(GL_DEPTH_BUFFER_BIT);

  uniformModel = omniShadowShader.GetModelLocation();
  uniformOmniLightPos = omniShadowShader.GetOmniLightPosLocation();
  uniformFarPlane = omniShadowShader.GetFarPlaneLocation();

  glUniform3f(uniformOmniLightPos,
      light->GetPosition().x,
      light->GetPosition().y,
      light->GetPosition().z);
  glUniform1f(uniformFarPlane, light->GetFarPlane());
  omniShadowShader.SetLightMatrices(light->CalculateLightTransform());

  RenderScene();

  // unbind
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void RenderPass(glm::mat4 viewMatrix, glm::mat4 projectionMatrix)
{
  shaderList[0].UseShader();

  uniformModel = shaderList[0].GetModelLocation();
  uniformProjection = shaderList[0].GetProjectionLocation();
  uniformView = shaderList[0].GetViewLocation();
  uniformEyePosition = shaderList[0].GetEyePositionLocation();
  uniformSpecularIntensity = shaderList[0].GetSpecularIntensityLocation();
  uniformShininess = shaderList[0].GetShininessLocation();

  glViewport(0, 0, 1024, 768);

  // Clear window
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // the Position of these two lines doesn't matter so long as they are done
  // before the very first draw
  glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
  glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(viewMatrix));
  glUniform3f(uniformEyePosition,
      camera.getCameraPosition().x,
      camera.getCameraPosition().y,
      camera.getCameraPosition().z);

  // Use our light source
  shaderList[0].SetDirectionalLight(&mainLight);
  shaderList[0].SetPointLights(pointLights, pointLightCount);
  shaderList[0].SetSpotLights(spotLights, spotLightCount);

  //shaderList[0].SetDirectionalLightTransform(&mainLight.CalculateLightTransform());
  glm::mat4 foo = mainLight.CalculateLightTransform();
  shaderList[0].SetDirectionalLightTransform(&foo);

  mainLight.GetShadowMap()->Read(GL_TEXTURE1);
  shaderList[0].SetTexture(0);
  shaderList[0].SetDirectionalShadowMap(1);

  glm::vec3 lowerLight = camera.getCameraPosition();
  lowerLight.y -= 0.3f;
  //spotLights[0].SetFlash(lowerLight, camera.getCameraDirection());

  RenderScene();
}

int main()
{
  mainWindow = Window(1024, 768);
  mainWindow.initialize();

  CreateObjects();
  CreateShaders();

  camera = Camera(glm::vec3(0.0f, 0.0f, 0.0f),
      glm::vec3(0.0f, 1.0f, 0.0f),
      -90.0f,
      0.0f,
      5.0f,
      0.5f);

  brickTexture = Texture("Textures/brick.png");
  brickTexture.LoadTextureA();

  dirtTexture = Texture("Textures/dirt.png");
  dirtTexture.LoadTextureA();

  plainTexture = Texture("Textures/plain.png");
  plainTexture.LoadTextureA();

  shinyMaterial = Material(4.0f, 256);
  dullMaterial = Material(0.3f, 4);

  xwing = Model();
  xwing.LoadModel("Models/x-wing.obj");

  blackhawk = Model();
  blackhawk.LoadModel("Models/uh60.obj");

  mainLight = DirectionalLight(
      2048, 2048,
      1.0f, 1.0f, 1.0f,
      0.1f, 0.3f,
      0.0f, -7.0f, -1.0f);

  // Setup spot lights
  pointLights[0] = PointLight(
      1024, 1024,
      0.01f, 100.0f,
      0.0f, 0.0f, 1.0f,
      0.0f, 0.1f,
      0.0f, 0.0f, 0.0f,
      0.3f, 0.2f, 0.1f);
  pointLightCount++;

  pointLights[1] = PointLight(
      1024, 1024,
      0.01f, 100.0f,
      0.0f, 1.0f, 0.0f,
      0.0f, 0.1f,
      -4.0f, 2.0f, 0.0f,
      0.3f, 0.1f, 0.1f);
  //pointLightCount++;

  // Setup point lights
  spotLights[0] = SpotLight(
      1024, 1024,
      0.01f, 100.0f,
      1.0f, 1.0f, 1.0f,
      0.0f, 2.0f,
      0.0f, 0.0f, 0.0f,
      0.0f, -1.0f, 0.0f,
      1.0f, 0.0f, 0.0f,
      20.0f);
  //spotLightCount++;

  spotLights[1] = SpotLight(
      1024, 1024,
      0.01f, 100.0f,
      1.0f, 1.0f, 1.0f,
      0.0f, 1.0f,
      0.0f, -1.5f, 0.0f,
      -100.0f, -1.0f, 0.0f,
      1.0f, 0.0f, 0.0f,
      20.0f);
  //spotLightCount++;

  // Prepare the projection matrix
  glm::mat4 projection = glm::perspective(
      glm::radians(60.0f),
      (GLfloat)mainWindow.getBufferWidth() / mainWindow.getBufferHeight(), 
      0.1f,
      100.0f);

  // loop until window closed
  while (!mainWindow.getShouldClose())
  {
    GLfloat now = glfwGetTime();
    deltaTime = now - lastTime;
    lastTime = now;

    // Get and handle user input events
    glfwPollEvents();

    // User input for the camera
    camera.keyControl(mainWindow.getKeys(), deltaTime);
    camera.mouseControl(mainWindow.getXChange(), mainWindow.getYChange());

    DirectionalShadowMapPass(&mainLight);

    // Point light shadows
    for (size_t i = 0; i < pointLightCount; i++)
    {
      OmniShadowMapPass(&pointLights[i]);
    }

    // Spot light shadows
    for (size_t i = 0; i < spotLightCount; i++)
    {
      OmniShadowMapPass(&spotLights[i]);
    }

    RenderPass(camera.calculateViewMatrix(), projection);

    mainWindow.swapBuffers();
  }

  return 0;
}
