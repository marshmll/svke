#version 450

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec3 inNormal;
layout(location = 3) in vec2 inUv;

layout(push_constant) uniform Push
{
    mat4 modelMatrix;
    mat4 normalMatrix;
}
push;

layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec3 fragPosWorld;
layout(location = 2) out vec3 fragNormalWorld;
layout(location = 3) out vec2 fragUv;

struct PointLight
{
    vec3 position;
    vec4 color; // w = intensity
};

layout(set = 0, binding = 0) uniform GlobalUbo
{
    mat4 projectionMatrix;
    mat4 viewMatrix;
    mat4 inverseViewMatrix;
    vec4 ambientLightColor;
    PointLight pointLights[10];
    int numLights;
}
ubo;

void main()
{
   vec4 positionWorld = push.modelMatrix * vec4(inPosition, 1.0);
   gl_Position = ubo.projectionMatrix * ubo.viewMatrix * positionWorld;

   fragColor = inColor;
   fragPosWorld = positionWorld.xyz;
   fragNormalWorld = normalize(mat3(push.normalMatrix) * inNormal);
   fragUv = inUv;
}
