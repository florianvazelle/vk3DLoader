#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 positions;
layout(location = 1) in vec3 normals;
layout(location = 2) in vec3 colors;
layout(location = 3) in vec2 texCoords;

struct UniformBufferObject {
  mat4 model;
  mat4 view;
  mat4 proj;
  mat4 depthBiasMVP;
  vec3 lightPos;
};

layout(binding = 0) uniform UBO { UniformBufferObject ubo; };

layout(location = 0) out vec3 outPosition;
layout(location = 1) out vec3 outNormal;
layout(location = 2) out vec4 outShadowCoord;
layout(location = 3) out vec3 outLightPos;
layout(location = 4) out vec3 outLightVec;
layout(location = 5) out vec3 outViewVec;
layout(location = 6) out vec2 outTexCoords;

out gl_PerVertex { vec4 gl_Position; };

const mat4 biasMat = mat4(0.5, 0.0, 0.0, 0.0, 0.0, 0.5, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.5, 0.5, 0.0, 1.0);

void main(void) {
  const vec3 cameraPos = vec3(2);

  vec3 ModPos = vec3(ubo.model * vec4(positions, 1.0));

  outPosition = ModPos;
  outNormal   = vec3(transpose(inverse(ubo.model)) * vec4(normals, 0.0));
  gl_Position = ubo.proj * ubo.view * vec4(ModPos, 1);

  outShadowCoord = (biasMat * ubo.depthBiasMVP) * vec4(ModPos, 1);
  outLightPos    = ubo.lightPos;

  vec3 lPos   = mat3(ubo.model) * ubo.lightPos;
  outLightVec = lPos - ModPos;
  outViewVec  = cameraPos - ModPos;

  outTexCoords= texCoords;
}
