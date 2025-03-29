#version 450

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec3 inNormal;
layout(location = 3) in vec2 inUv;

layout(push_constant) uniform Push
{
    mat4 transform;
    mat4 normalMatrix;
} push;

layout(location = 0) out vec3 fragColor;

const vec3 DIRECTION_TO_LIGHT = normalize(vec3(1.0, -3.0, -1.0));
const float AMBIENT = 0.02;

void main()
{
    gl_Position = push.transform * vec4(inPosition, 1.0);

    vec3 normalWorldSpace = normalize(mat3(push.normalMatrix) * inNormal);
    float lightIntensity = AMBIENT + max(dot(normalWorldSpace, DIRECTION_TO_LIGHT), 0);

    fragColor = lightIntensity * inColor;
}
