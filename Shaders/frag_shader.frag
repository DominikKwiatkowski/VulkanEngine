#version 450

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec3 fragPosWorld;
layout(location = 2) in vec3 fragNormalWorld;

layout(location = 0) out vec4 outColor;

layout(push_constant) uniform Push{
	mat4 modelMatrix;
	mat4 normalMatrix;
}push;

layout(set = 0, binding = 0) uniform GlobalUbo {
  mat4 projectionMatrix;
  mat4 viewMatrix;
  vec4 ambientLight;
  vec4 lightPosition;
  vec4 lightColor;
} ubo;

void main()
{
	vec3 directionToLight = ubo.lightPosition.xyz - fragPosWorld;
	float attenuation = 1.0 / dot(directionToLight,directionToLight);

	vec3 lightColor = ubo.lightPosition.xyz * ubo.lightPosition.w * attenuation;
	vec3 ambientLight = ubo.ambientLight.xyz * ubo.ambientLight.w;
	vec3 diffuseLight = lightColor * max(dot(normalize(fragNormalWorld),normalize(directionToLight)), 0);

	outColor = vec4((diffuseLight + ambientLight) * fragColor,1.0);
}