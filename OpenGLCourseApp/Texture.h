#pragma once

#include <GL/glew.h>

// image loading
#include "stb_image.h"

class Texture
{
  public:
    Texture();
    Texture(char* fileLoc);
    ~Texture();

    void LoadTexture();
    void UseTexture();
    void ClearTexture();

  private:
    GLuint textureID;
    int width, height, bitDepth;

    char* fileLocation;
};
