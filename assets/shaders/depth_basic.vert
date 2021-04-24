#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 positions;
layout(location = 1) in vec3 normals;
layout(location = 2) in vec3 colors;
layout(location = 3) in vec2 texCoords;

struct UniformBufferObject {
  mat4 depthMVP;
};

layout(binding = 0) uniform UBO { UniformBufferObject ubo; };

out gl_PerVertex { vec4 gl_Position; };

void main() { gl_Position = ubo.depthMVP * vec4(positions, 1.0); }