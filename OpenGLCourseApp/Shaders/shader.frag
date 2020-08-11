#version 330

in vec4 vCol;
in vec2 TexCoord;
in vec3 Normal;
in vec3 FragPos;
in vec4 DirectionalLightSpacePos;

out vec4 color;

const int MAX_POINT_LIGHTS = 3;
const int MAX_SPOT_LIGHTS = 3;

struct Light
{
  vec3 color;
  float ambientIntensity;
  float diffuseIntensity;
};

struct DirectionalLight
{
  Light base;
  vec3 direction;
};

struct PointLight
{
  Light base;
  vec3 position;
  float constant;
  float linear;
  float exponent;
};

struct SpotLight
{
  PointLight base;
  vec3 direction;
  float edge;
};

struct OmniShadowMap
{
  samplerCube shadowMap;
  float farPlane;
};

struct Material
{
  float specularIntensity;
  float shininess;
};

uniform int pointLightCount;
uniform int spotLightCount;

uniform DirectionalLight directionalLight;
uniform PointLight pointLights[MAX_POINT_LIGHTS];
uniform SpotLight spotLights[MAX_SPOT_LIGHTS];

uniform sampler2D theTexture;
uniform sampler2D directionalShadowMap;
// remember, we'll have a omniShadowMap for each poit and spot light in our scene
uniform OmniShadowMap omniShadowMaps[MAX_POINT_LIGHTS + MAX_SPOT_LIGHTS];

uniform Material material;

uniform vec3 eyePosition;

float CalcDirectionalShadowFactor(DirectionalLight light)
{
  vec3 projCoords = DirectionalLightSpacePos.xyz / DirectionalLightSpacePos.w;
  projCoords = (projCoords * 0.5) + 0.5; // keep values between 0 and 1
 
  float current = projCoords.z;

  vec3 normal = normalize(Normal);
  vec3 lightDir = normalize(light.direction);
  // prevent banding from shadows
  float bias = max(0.05f * (1.0f - dot(normal, lightDir)), 0.005f);

  float shadow = 0.0f;
  vec2 texelSize = 1.0f / textureSize(directionalShadowMap, 0);
  for (int x = -1; x <= 1; ++x)
  {
    for (int y = -1; y <= 1; ++y)
    {
      // r is for depth
      float pcfDepth = texture(directionalShadowMap, projCoords.xy + vec2(x, y) * texelSize).r;
      shadow += current - bias > pcfDepth ? 1.0f : 0.0f;
    }
  }

  shadow /= 9.0f;

  // if beyond far plane, don't place shadow
  if (projCoords.z > 1.0f)
  {
    shadow = 0.0f;
  }

  return shadow;
}

float CalcOmniShadowFactor(PointLight light, int shadowIndex)
{
  vec3 fragToLight = FragPos - light.position;
  float closest = texture(omniShadowMaps[shadowIndex].shadowMap, fragToLight).r;

  // scale up from the 0 to 1 range it was at
  closest *= omniShadowMaps[shadowIndex].farPlane;

  float current = length(fragToLight);

  float bias = 0.05;
  float shadow = current - bias > closest ? 1.0f : 0.0f;

  return shadow;
}

vec4 CalcLightByDirection(Light light, vec3 direction, float shadowFactor)
{
  // calculate ambient color
  vec4 ambientColor = vec4(light.color, 1.0f) * light.ambientIntensity;

  // calculate diffuse color
  float diffuseFactor = max(dot(normalize(Normal), normalize(direction)), 0.0f);
  vec4 diffuseColor = vec4(light.color, 1.0f) * light.diffuseIntensity * diffuseFactor;

  // calculate specular
  vec4 specularColor = vec4(0, 0, 0, 0);

  // Remember, a specular light should only show up if there is a diffuse factor!
  // If no light hits the object then there shouldn't be a specular.
  if (diffuseFactor > 0.0f)
  {
    vec3 fragToEye = normalize(eyePosition - FragPos);
    vec3 reflectedVertex = normalize(reflect(direction, normalize(Normal)));

    float specularFactor = dot(fragToEye, reflectedVertex);
    if (specularFactor > 0.0f)
    {
      specularFactor = pow(specularFactor, material.shininess);
      specularColor = vec4(light.color * material.specularIntensity * specularFactor, 1.0f);
    }
  }

  return ambientColor + (1.0f - shadowFactor) * (diffuseColor + specularColor);
}

vec4 CalcDirectionalLight()
{
  float shadowFactor = CalcDirectionalShadowFactor(directionalLight);
  return CalcLightByDirection(
      directionalLight.base,
      directionalLight.direction,
      shadowFactor);
}

vec4 CalcPointLight(PointLight pLight, int shadowIndex)
{
  vec3 direction = FragPos - pLight.position;
  float distance = length(direction);
  direction = normalize(direction);

  float shadowFactor = CalcOmniShadowFactor(pLight, shadowIndex);

  vec4 color = CalcLightByDirection(pLight.base, direction, shadowFactor);
  float attenuation = pLight.exponent * distance * distance +
    pLight.linear * distance +
    pLight.constant;

  return color / attenuation;
}

vec4 CalcPointLights()
{
  vec4 totalColor = vec4(0, 0, 0, 0);
  for (int i = 0; i < pointLightCount; i++)
  {
    totalColor += CalcPointLight(pointLights[i], i);
  }

  return totalColor;
}

vec4 CalcSpotLight(SpotLight sLight, int shadowIndex)
{
  vec3 rayDirection = normalize(FragPos - sLight.base.position);
  float slFactor = dot(rayDirection, sLight.direction);

  // check if fragment is within the cone of the spot light
  if(slFactor > sLight.edge)
  {
    vec4 color = CalcPointLight(sLight.base, shadowIndex);
    return color * (1.0f - (1.0f - slFactor) / (1.0f - sLight.edge));
  }

  return vec4(0, 0, 0, 0);
}

vec4 CalcSpotLights()
{
  vec4 totalColor = vec4(0, 0, 0, 0);
  for (int i = 0; i < spotLightCount; i++)
  {
    // remember, we store point and spot lights in the same array.
    // as such, we need to offset since we store point lights first
    totalColor += CalcSpotLight(spotLights[i], i + pointLightCount);
  }

  return totalColor;
}

void main()
{
  vec4 finalColor = CalcDirectionalLight();
  finalColor += CalcPointLights();
  finalColor += CalcSpotLights();

  color = texture(theTexture, TexCoord) * finalColor;
}
