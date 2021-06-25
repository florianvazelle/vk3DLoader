#version 450
#extension GL_ARB_separate_shader_objects : enable

/**
 * in
 */
layout(binding = 3) uniform sampler2D texturename;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec4 inShadowCoord;
layout(location = 3) in vec3 inLightPos;
layout(location = 4) in vec3 inLightVec;
layout(location = 5) in vec3 inViewVec;
layout(location = 6) in vec2 inTexCoords;

// layout(constant_id = 0) const bool enabledShadowMap = true;

// peut etre input_attachment_index = index in pInputAttachments
// layout(input_attachment_index = 0, binding = 1) uniform subpassInput inputColor;
// layout(input_attachment_index = 0, binding = 1) uniform subpassInput inputDepth;

/**
 * uniform
 */

layout(binding = 1) uniform sampler2D shadowMap;

struct Material {
  vec3 AmbientColor;
  vec3 DiffuseColor;
  vec3 SpecularColor;
  float Shininess;
};

layout(binding = 2) uniform M { Material u_Material; };

/**
 * out
 */

layout(location = 0) out vec4 outColor;

/**
 * Functions
 */

// calcul du facteur diffus, suivant la loi du cosinus de Lambert
float Lambert(vec3 N, vec3 L) { return max(0.0, dot(N, L)); }

// calcul du facteur speculaire, methode de Phong
float Phong(vec3 N, vec3 L, vec3 V, float shininess) {
  // reflexion du vecteur incident I (I = -L)
  // suivant la loi de ibn Sahl / Snell / Descartes
  vec3 R = reflect(-L, N);
  return pow(max(0.0, dot(R, V)), shininess);
}

float textureProj(vec4 shadowCoord, vec2 off) {
  float shadow = 1.0;
  if (shadowCoord.z > -1.0 && shadowCoord.z < 1.0) {
    float dist = texture(shadowMap, shadowCoord.st + off).r;
    if (shadowCoord.w > 0.0 && dist < shadowCoord.z) {
      shadow = 0.1;
    }
  }
  return shadow;
}

// Percentage Close Filter
float filterPCF(vec4 sc) {
  ivec2 texDim = textureSize(shadowMap, 0);
  float scale  = 1.5;
  float dx     = scale * 1.0 / float(texDim.x);
  float dy     = scale * 1.0 / float(texDim.y);

  float shadowFactor = 0.0;

  for (int x = -1; x <= 1; x++) {
    for (int y = -1; y <= 1; y++) {
      shadowFactor += textureProj(sc, vec2(dx * x, dy * y));
    }
  }
  return shadowFactor / 9;
}

/**
 * Main
 */

void main() {
  const vec3 lightColor = vec3(0.2, 0.8, 0.5);

  vec3 V = normalize(inViewVec);
  vec3 N = normalize(inNormal);
  vec3 L = normalize(inLightVec);

  float dist        = length(inLightVec);
  dist              = dist * dist;
  float attenuation = 1.0 / dist;

  vec3 diffuseColor  = u_Material.DiffuseColor * Lambert(N, L);
  vec3 specularColor = u_Material.SpecularColor * Phong(N, L, V, u_Material.Shininess);
  vec3 directColor   = (diffuseColor + specularColor) * lightColor * attenuation;

  vec3 indirectColor = u_Material.AmbientColor;
 // vec3 color         = directColor + indirectColor;
  vec3 color = texture(texturename, inTexCoords).xyz;

  // Apply shadow mapping
  float visibility = filterPCF(inShadowCoord / inShadowCoord.w);
  color *= visibility;

  // correction gamma
  color = pow(color, vec3(1.0 / 2.2));

  outColor =  vec4(color, 1.0);
  // outColor =  texture(texturename, inTexCoords); 
}