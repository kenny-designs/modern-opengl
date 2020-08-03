#version 330

in vec4 vCol;
in vec2 TexCoord;
in vec3 Normal;

out vec4 color;

struct DirectionalLight
{
  vec3 color;
  float ambientIntensity;
  vec3 direction;
  float diffuseIntensity;
};

uniform sampler2D theTexture;
uniform DirectionalLight directionalLight;

void main()
{
  // calculate ambient color
  vec4 ambientColor = vec4(directionalLight.color, 1.0f) * directionalLight.ambientIntensity;

  // calculate diffuse color
  float diffuseFactor = max(dot(normalize(Normal), normalize(directionalLight.direction)), 0.0f);
  vec4 diffuseColor = vec4(directionalLight.color, 1.0f) * directionalLight.diffuseIntensity * diffuseFactor;

  // phong shade it up!
  color = texture(theTexture, TexCoord) * (ambientColor + diffuseColor);
}
