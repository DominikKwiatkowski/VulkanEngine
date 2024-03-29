#version 450

layout(location = 0) in vec2 fragOffset;

layout(location = 0) out vec4 outColor;

layout(set = 0, binding = 0) uniform GlobalUbo {
  mat4 projectionMatrix;
  mat4 viewMatrix;
  vec4 ambientLight;
  vec4 lightPosition;
  vec4 lightColor;
} ubo;

void main()
{
	if(sqrt(dot(fragOffset,fragOffset)) > 1)
		discard;
	outColor = vec4(ubo.lightColor.xyz,1);
}