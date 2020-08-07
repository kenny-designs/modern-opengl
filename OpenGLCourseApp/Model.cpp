#include "Model.h"

Model::Model(){}

void Model::LoadModel(const std::string& fileName)
{
  Assimp::Importer importer;
  const aiScene* scene = importer.ReadFile(
      fileName,
      aiProcess_Triangulate | 
      aiProcess_FlipUVs |
      aiProcess_GenSmoothNormals |
      aiProcess_JoinIdenticalVertices);

  if (!scene)
  {
    printf("Model (%s) failed to load: %s", fileName, importer.GetErrorString());
    return;
  }

  LoadNode(scene->mRootNode, scene);
  LoadMaterials(scene);
}

void Model::RenderModel()
{
  for (size_t i = 0; i < meshList.size(); i++)
  {
    unsigned int materialIndex = meshToTex[i];

    if (materialIndex < textureList.size() && textureList[materialIndex])
    {
      textureList[materialIndex]->UseTexture();
    }

    meshList[i]->RenderMesh();
  }
}

void Model::ClearModel()
{
  for (size_t i = 0; i < meshList.size(); i++)
  {
    if (meshList[i])
    {
      delete meshList[i];
      meshList[i] = nullptr;
    }
  }

  for (size_t i = 0; i < textureList.size(); i++)
  {
    if (textureList[i])
    {
      delete textureList[i];
      textureList[i] = nullptr;
    }
  }
}

void Model::LoadNode(aiNode* node, const aiScene* scene)
{
  for (size_t i = 0; i < node->mNumMeshes; i++)
  {
    LoadMesh(scene->mMeshes[node->mMeshes[i]], scene);
  }

  // now for the recursive part of the code
  for (size_t i = 0; i < node->mNumChildren; i++)
  {
    LoadNode(node->mChildren[i], scene);
  }
}

void Model::LoadMesh(aiMesh* mesh, const aiScene* scene)
{
  std::vector<GLfloat> vertices;
  std::vector<unsigned int> indices;

  for (size_t i = 0; i < mesh->mNumVertices; i++)
  {
    // first we insert the vertices
    vertices.insert(vertices.end(),
        { mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z });

    // check if we have any textures. If so, add them.
    if (mesh->mTextureCoords[0])
    {
      vertices.insert(vertices.end(),
          { mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y });
    }
    // even if there are no texture coords, we still need to put something
    else
    {
      vertices.insert(vertices.end(), { 0.0f, 0.0f });
    }

    // now we insert the normals
    // also, in the vertex shader we normally put negative for the normals
    // however, we did not. So, we must add negatives here!
    vertices.insert(vertices.end(),
        { -mesh->mNormals[i].x, -mesh->mNormals[i].y, -mesh->mNormals[i].z });
  }

  // next up, we do the faces!
  // also, each face has 3 values. Y'know, cause meshes are made out of triangles
  for (size_t i = 0; i < mesh->mNumFaces; i++)
  {
    aiFace face = mesh->mFaces[i];
    for (size_t j = 0; j < face.mNumIndices; j++)
    {
      indices.push_back(face.mIndices[j]);
    }
  }

  Mesh* newMesh = new Mesh();
  // remember, arrays are sequential in memory in C++ so we can go
  // &vertices[0] to get the location of the first element in memory for the array
  newMesh->CreateMesh(&vertices[0], &indices[0], vertices.size(), indices.size());
  meshList.push_back(newMesh);
  meshToTex.push_back(mesh->mMaterialIndex);
}

void Model::LoadMaterials(const aiScene* scene)
{
  textureList.resize(scene->mNumMaterials);

  for (size_t i = 0; i < scene->mNumMaterials; i++)
  {
    aiMaterial* material = scene->mMaterials[i];

    textureList[i] = nullptr;

    if (material->GetTextureCount(aiTextureType_DIFFUSE))
    {
      aiString path;
      if (material->GetTexture(aiTextureType_DIFFUSE, 0, &path) == AI_SUCCESS)
      {
        // we do the following in case the person that saved the model used
        // a direct filepath
        int idx = std::string(path.data).rfind("\\");
        std::string filename = std::string(path.data).substr(idx + 1);
        std::string texPath = std::string("Textures/") + filename;

        textureList[i] = new Texture(texPath.c_str());

        // assuming there are no alpha channels
        if (!textureList[i]->LoadTexture())
        {
          printf("Failed to load texture at: %s\n", texPath);
          delete textureList[i];
          textureList[i] = nullptr;
        }
      }
    }

    // if failed to load in a texture, or if there just wasn't one to begin with,
    // we'll use a default texture
    if (!textureList[i])
    {
      textureList[i] = new Texture("Textures/plain.png");
      textureList[i]->LoadTextureA();
    }
  }
}


Model::~Model(){}
