#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 fragPosition;
layout(location = 1) in vec3 fragModPos;
layout(location = 2) in vec4 fragShadowCoord;
layout(location = 3) in vec3 fragLightPos;
layout(location = 4) in vec2 fragTexCoords;

// layout(constant_id = 0) const bool enabledShadowMap = true;

// layout(binding = 1) uniform sampler2D shadowMap;

// peut etre input_attachment_index = index in pInputAttachments
// layout(input_attachment_index = 0, binding = 1) uniform subpassInput inputColor;
layout(input_attachment_index = 0, binding = 1) uniform subpassInput inputDepth;

struct Material {
  vec3 AmbientColor;
  vec3 DiffuseColor;
  vec3 SpecularColor;
  float Shininess;
};

layout(binding = 2) uniform M { Material u_Material; };

layout(location = 0) out vec4 outColor;

const vec3 lightColor   = vec3(1.0, 1.0, 1.0);
const float lightPower  = 40.0;
const float screenGamma = 2.2;

vec3 diffuse(vec3 N, vec3 L) {
  vec3 nDir = normalize(N);
  vec3 lDir = normalize(L);

  return vec3(1.0, 0.0, 1.0) * u_Material.DiffuseColor * max(0.0, dot(nDir, lDir));
}

vec3 specular(vec3 N, vec3 L) {
  vec3 V = normalize(-fragModPos);

  // blinn-phong
  vec3 H = normalize(L + V);
  return vec3(1.0, 0.2, 0.3) * u_Material.SpecularColor * max(pow(dot(N, H), u_Material.Shininess), 0.0);
}

float LinearizeDepth(float depth) {
  float n = 0.1;    // camera z near
  float f = 128.0;  // camera z far
  float z = depth;
  return (2.0 * n) / (f + n - z * (f - n));
}

void main() {
  vec3 N = normalize(fragPosition);
  vec3 L = fragLightPos - fragModPos;

  float bias = 0.005 * tan(acos(dot(N, L)));
  bias       = clamp(bias, 0, 0.01);

  // float visibility = 1.0;
  // if (texture(shadowMap, fragShadowCoord.xy).z < fragShadowCoord.z - bias) {
  //   visibility = 0.5;
  // }

  // Percentage Close Filter
  float visibility = 0.0;
  // vec2 texelSize   = 1.0 / textureSize(shadowMap, 0);
  for (int x = -1; x <= 1; ++x) {
    for (int y = -1; y <= 1; ++y) {
      // float depth = texture(shadowMap, fragShadowCoord.xy + vec2(x, y) * texelSize).z; // pcfDepth
      float depth = subpassLoad(inputDepth).r;
      visibility += fragShadowCoord.z > depth ? 0.5 : 0.0;
    }
  }
  visibility /= 9.0;

  visibility = 1.0 - visibility;

  float distance = length(L);
  distance       = distance * distance;
  L              = normalize(L);

  vec3 colorLinear = u_Material.AmbientColor + (diffuse(N, L) * lightColor * lightPower / distance);
  // if (m_enabledShadowMap) {
  colorLinear *= visibility;
  // }
  vec3 colorGammaCorrected = pow(colorLinear, vec3(1.0 / screenGamma));
  outColor                 = vec4(colorGammaCorrected, 1.0);

  // debug
  // float depth = subpassLoad(inputDepth).r;
  // outColor    = vec4(vec3(1.0 - LinearizeDepth(depth)), 1.0);
}