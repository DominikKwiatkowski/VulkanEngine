#version 450

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 color;
layout(location = 2) in vec3 normal;
layout(location = 3) in vec2 texCord;

layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec3 fragPosWorld;
layout(location = 2) out vec3 fragNormalWorld;

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

void main(){
	vec4 vertexPosition = push.modelMatrix * vec4(position, 1.0);
	gl_Position = ubo.projectionMatrix * ubo.viewMatrix * vertexPosition;

	fragNormalWorld = normalize(mat3(push.normalMatrix) * normal);
	fragPosWorld = vertexPosition.xyz;
	fragColor = color;
}
