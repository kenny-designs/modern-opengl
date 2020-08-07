#pragma once

#include <GL/glew.h>

// image loading
#include "stb_image.h"

class Texture
{
  public:
    Texture();
    Texture(const char* fileLoc);
    ~Texture();

    bool LoadTexture();   // load non-alpha
    bool LoadTextureA();  // load with alpha

    void UseTexture();
    void ClearTexture();

  private:
    GLuint textureID;
    int width, height, bitDepth;

    const char* fileLocation;
};
