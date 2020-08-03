#version 330

layout (location = 0) in vec3 pos;
layout (location = 1) in vec2 tex;
layout (location = 2) in vec3 norm;

out vec4 vCol;
out vec2 TexCoord;
out vec3 Normal;

uniform mat4 model;
uniform mat4 projection;
uniform mat4 view;

void main()
{
  gl_Position = projection * view * model * vec4(pos, 1.0f);
  vCol = vec4(clamp(pos, 0.0f, 1.0f), 1.0f);

  TexCoord = tex;

  // Remember, the last column and row of our model deals with
  // rotation and scale. We don't need them! So, mat3().
  // The inverse and transpose also helps non-uniform scaling.
  //
  // The reason we need to do all this then multiply by norm is
  // so we can account for scaling and rotation!
  Normal = mat3(transpose(inverse(model))) * norm;
}
