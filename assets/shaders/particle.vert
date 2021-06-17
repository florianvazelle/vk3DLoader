#version 450

layout (location = 0) in vec4 inPos;
layout (location = 1) in vec4 inVel;

layout (location = 0) out float outGradientPos;

layout (binding = 2) uniform UBO {
  mat4 model;
  mat4 view;
  mat4 proj;
	vec2 screendim;
} ubo;

out gl_PerVertex {
	vec4 gl_Position;
	float gl_PointSize;
};

void main () {
	const float spriteSize = 0.005 * inPos.w; // Point size influenced by mass (stored in inPos.w);

	vec4 eyePos = ubo.view * (ubo.model * vec4(inPos.x, inPos.y, inPos.z, 1.0));
	vec4 projectedCorner = ubo.proj * vec4(0.5 * spriteSize, 0.5 * spriteSize, eyePos.z, eyePos.w);
	gl_PointSize = clamp(ubo.screendim.x * projectedCorner.x / projectedCorner.w, 1.0, 128.0);

	gl_Position = ubo.proj * eyePos;

	outGradientPos = inVel.w;
}
