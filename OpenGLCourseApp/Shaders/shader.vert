#version 330

layout (location = 0) in vec3 pos;
layout (location = 1) in vec2 tex;
layout (location = 2) in vec3 norm;

out vec4 vCol;
out vec2 TexCoord;
out vec3 Normal;
out vec3 FragPos;
out vec4 DirectionalLightSpacePos;

uniform mat4 model;
uniform mat4 projection;
uniform mat4 view;
uniform mat4 directionalLightTransform;

void main()
{
  gl_Position = projection * view * model * vec4(pos, 1.0f);
  DirectionalLightSpacePos = directionalLightTransform * model * vec4(pos, 1.0f);

  vCol = vec4(clamp(pos, 0.0f, 1.0f), 1.0f);

  TexCoord = tex;

  // Remember, the last column and row of our model deals with
  // rotation and scale. We don't need them! So, mat3().
  // The inverse and transpose also helps non-uniform scaling.
  //
  // The reason we need to do all this then multiply by norm is
  // so we can account for scaling and rotation!
  Normal = mat3(transpose(inverse(model))) * norm;

  // Firstly, we want the position of the fragment within the world.
  // As such, we don't multiply by the projection and view matrices.
  //
  // Second, we need a vec3 and not a vec4! We can get a vec3 via
  // swizzling (or is it swivel?). That is why we use the .xyz at the end.
  FragPos = (model * vec4(pos, 1.0f)).xyz;
}
