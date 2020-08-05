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
#include "Material.h"

// Window dimensions
const float toRadians = 3.14159265f / 180.0f;

Window mainWindow;
std::vector<Mesh*> meshList;
std::vector<Shader> shaderList;
Camera camera;

Texture brickTexture;
Texture dirtTexture;
Texture plainTexture;

Material shinyMaterial;
Material dullMaterial;

DirectionalLight mainLight;
PointLight pointLights[MAX_POINT_LIGHTS];

GLfloat deltaTime = 0.0f;
GLfloat lastTime = 0.0f;

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
  brickTexture.LoadTexture();

  dirtTexture = Texture("Textures/dirt.png");
  dirtTexture.LoadTexture();

  plainTexture = Texture("Textures/plain.png");
  plainTexture.LoadTexture();

  shinyMaterial = Material(4.0f, 256);
  dullMaterial = Material(0.3f, 4);

  mainLight = DirectionalLight(1.0f, 1.0f, 1.0f,
      0.0f, 0.0f,
      2.0f, -1.0f, -2.0f);

  unsigned int pointLightCount = 0;
  pointLights[0] = PointLight(
      0.0f, 0.0f, 1.0f,
      0.0f, 1.0f,
      0.0f, 0.0f, 0.0f,
      0.3f, 0.2f, 0.1f);
  pointLightCount++;

  pointLights[1] = PointLight(
      0.0f, 1.0f, 0.0f,
      0.0f, 1.0f,
      -4.0f, 2.0f, 0.0f,
      0.3f, 0.1f, 0.1f);
  pointLightCount++;

  GLuint uniformProjection = 0,
         uniformModel = 0,
         uniformView = 0,
         uniformEyePosition = 0,
         uniformSpecularIntensity = 0,
         uniformShininess = 0;

  // Prepare the projection matrix
  glm::mat4 projection = glm::perspective(
      45.0f,
      mainWindow.getBufferWidth() / mainWindow.getBufferHeight(), 
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

    // Clear window
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // use our shader program
    shaderList[0].UseShader();
    uniformModel = shaderList[0].GetModelLocation();
    uniformProjection = shaderList[0].GetProjectionLocation();
    uniformView = shaderList[0].GetViewLocation();
    uniformEyePosition = shaderList[0].GetEyePositionLocation();
    uniformSpecularIntensity = shaderList[0].GetSpecularIntensityLocation();
    uniformShininess = shaderList[0].GetShininessLocation();

    // Use our light source
    shaderList[0].SetDirectionalLight(&mainLight);
    shaderList[0].SetPointLights(pointLights, pointLightCount);

    // the Position of these two lines doesn't matter so long as they are done
    // before the very first draw
    glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
    glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));
    glUniform3f(uniformEyePosition,
        camera.getCameraPosition().x,
        camera.getCameraPosition().y,
        camera.getCameraPosition().z);

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

    plainTexture.UseTexture();
    shinyMaterial.UseMaterial(uniformSpecularIntensity, uniformShininess);
    meshList[2]->RenderMesh();

    // remove our shader program
    glUseProgram(0);

    mainWindow.swapBuffers();
  }

  return 0;
}
