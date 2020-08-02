#include "Texture.h"

Texture::Texture()
{
  textureID = 0;
  width = 0;
  height = 0;
  bitDepth = 0;
  fileLocation = "";
}

Texture::Texture(char* fileLoc)
{
  textureID = 0;
  width = 0;
  height = 0;
  bitDepth = 0;
  fileLocation = fileLoc;
}

void Texture::LoadTexture()
{
  unsigned char* texData = stbi_load(fileLocation, &width, &height, &bitDepth, 0);
  if (!texData)
  {
    printf("Failed to find %s\n", fileLocation);
    return;
  }

  glGenTextures(1, &textureID);
  glBindTexture(GL_TEXTURE_2D, textureID);

  // repeat on the s (x-axis) and y (y-axis)
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

  // blend the pixels as we move closer to the image
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

  // same as above but as we move further away
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  // for the unsigned byte, remember that char's are just bytes of data!
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, texData);

  // now generate the mipmaps!
  glGenerateMipmap(GL_TEXTURE_2D);

  // unbind texture
  glBindTexture(GL_TEXTURE_2D, 0);
  stbi_image_free(texData);
}

void Texture::UseTexture()
{
  // this is referring to the Texture Unit 0
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, textureID);
}

void Texture::ClearTexture()
{
  glDeleteTextures(1, &textureID);
  textureID = 0;
  width = 0;
  height = 0;
  bitDepth = 0;
  fileLocation = "";
}

Texture::~Texture()
{
  ClearTexture();
}

