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

layout(location = 0) out vec3 fragPosition;
layout(location = 1) out vec3 fragModPos;
layout(location = 2) out vec4 fragShadowCoord;
layout(location = 3) out vec3 fragLightPos;
layout(location = 4) out vec2 fragTexCoords;

out gl_PerVertex { vec4 gl_Position; };

void main(void) {
  vec3 mp      = vec3(ubo.model * vec4(positions, 1.0));
  fragPosition = transpose(inverse(mat3(ubo.model))) * normals;
  fragModPos   = mp.xyz;
  gl_Position  = ubo.proj * ubo.view * vec4(mp, 1);

  fragShadowCoord = ubo.depthBiasMVP * vec4(mp, 1);
  fragLightPos    = ubo.lightPos;
  fragTexCoords   = texCoords;
}