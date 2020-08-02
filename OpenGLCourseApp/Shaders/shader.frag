#version 330

in vec4 vCol;
in vec2 TexCoord;

out vec4 color;

uniform sampler2D theTexture;

void main()
{
  // we can have rainbows!
  // color = vCol;

  // textures with rainbows!
  // color = texture(theTexture, TexCoord) * vCol;

  // textures by themselves
  color = texture(theTexture, TexCoord);
}
