#include "Mesh.h"

Mesh::Mesh()
{
  VAO = 0;
  VBO = 0;
  IBO = 0;
  indexCount = 0;
}

void Mesh::CreateMesh(GLfloat *vertices,
    unsigned int *indices,
    unsigned int numOfVertices,
    unsigned int numOfIndices)
{
  indexCount = numOfIndices; 

  // adds one vertex array to the VRAM and obtain the ID for it
  glGenVertexArrays(1, &VAO);
  glBindVertexArray(VAO);

  // setup the IBO
  // The IBO and VBO are connected through the VAO by the way!
  glGenBuffers(1, &IBO);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO); // element and index are interchangable here
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices[0]) * numOfIndices, indices, GL_STATIC_DRAW);

  // similar to the VAO, we create a single buffer and obtain the ID for it
  glGenBuffers(1, &VBO);
  // there are various targets we can bind the VBO to. Here, we'll bind to the
  // GL_ARRAY_BUFFER 
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  // GL_STATIC_DRAW means that we don't plan on changing the values within the
  // vertices array. If you do intend to, then use GL_DYNAMIC_DRAW.
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices[0]) * numOfVertices, vertices, GL_STATIC_DRAW);

  // a good little bit happening here. Refer to the documentation
  // we do the *5 because we added more to vertices[] to account for texture data
  // in addition, the first 0 is for position in our layout.
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertices[0]) * 5, 0);
  glEnableVertexAttribArray(0);

  // 1 is for texture coordinates layout,
  // 2 because we have the u and w coordinates to textures,
  // stride is 5,
  // and offset is 3 because we start looking for texture coords at the 3rd point
  // also, the last param needs to be a pointer. We typecast with void
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE,
      sizeof(vertices[0]) * 5,
      (void*)(sizeof(vertices[0]) * 3));
  glEnableVertexAttribArray(1);

  // Unbind the VAO, VBO, and IBO
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void Mesh::RenderMesh()
{
  // bind our VAO
  glBindVertexArray(VAO);

  // bind our IBO
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);

  // draw
  glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);

  // unbind our IBO
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

  // unbind our VAO
  glBindVertexArray(0);
}

void Mesh::ClearMesh()
{
  if (IBO != 0)
  {
    glDeleteBuffers(1, &IBO);
    IBO = 0;
  }

  if (VBO != 0)
  {
    glDeleteBuffers(1, &VBO);
    VBO = 0;
  }

  if (VAO != 0)
  {
    glDeleteVertexArrays(1, &VAO);
    VAO = 0;
  }

  indexCount = 0;
}

Mesh::~Mesh()
{
  ClearMesh();
}
