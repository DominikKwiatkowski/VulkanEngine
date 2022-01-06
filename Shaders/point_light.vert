#version 450

const vec2 OFFSETS[6] = vec2[](
  vec2(-1.0, -1.0),
  vec2(-1.0, 1.0),
  vec2(1.0, -1.0),
  vec2(1.0, -1.0),
  vec2(-1.0, 1.0),
  vec2(1.0, 1.0)
);

layout(set = 0, binding = 0) uniform GlobalUbo {
  mat4 projectionMatrix;
  mat4 viewMatrix;
  vec4 ambientLight;
  vec4 lightPosition;
  vec4 lightColor;
} ubo;

layout (location = 0) out vec2 fragOffset;

const float LIGHT_RADIOUS = 0.1;

void main(){
	fragOffset = OFFSETS[gl_VertexIndex];
	vec3 cameraRightWorld = {ubo.viewMatrix[0][0], ubo.viewMatrix[1][0],ubo.viewMatrix[2][0]};
	vec3 cameraUpWorld = {ubo.viewMatrix[0][1], ubo.viewMatrix[1][1],ubo.viewMatrix[2][1]};

	vec3 positionWorld = ubo.lightPosition.xyz
	+ LIGHT_RADIOUS * fragOffset.x * cameraRightWorld
	+ LIGHT_RADIOUS * fragOffset.y * cameraUpWorld;

	gl_Position = ubo.projectionMatrix * ubo.viewMatrix * vec4(positionWorld,1);
}