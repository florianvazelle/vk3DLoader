#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(binding = 0) uniform UniformBufferObject {
  mat4 model;
  mat4 view;
  mat4 proj;
} ubo;

layout(location = 0) in vec3 positions;
layout(location = 1) in vec3 colors;
layout(location = 2) in vec2 texCoords;

layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec2 fragTexCoord;

out gl_PerVertex { vec4 gl_Position; };

void main() {
  gl_Position  = ubo.proj * ubo.view * ubo.model * vec4(positions, 1.0);
  fragTexCoord = texCoords;
  fragColor    = colors;
}