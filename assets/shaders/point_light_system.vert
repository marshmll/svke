#version 450

layout(location = 0) out vec2 fragOffset;

struct PointLight
{
    vec3 position;
    vec4 color; // w = intensity
};

layout(set = 0, binding = 0) uniform GlobalUbo
{
    mat4 projectionMatrix;
    mat4 viewMatrix;
    vec4 ambientLightColor;
    PointLight pointLights[10];
    int numLights;
}
ubo;

layout(push_constant) uniform Push
{
    vec3 position;
    vec4 color;
    float radius;
}
push;

const vec2 OFFSETS[6] =
    vec2[](vec2(-1.0, -1.0), vec2(-1.0, 1.0), vec2(1.0, -1.0), vec2(1.0, -1.0), vec2(-1.0, 1.0), vec2(1.0, 1.0));

void main()
{
    fragOffset = OFFSETS[gl_VertexIndex];

    vec4 lightInCameraSpace = ubo.viewMatrix * vec4(push.position, 1.0);
    vec4 positionInCameraSpace = lightInCameraSpace + push.radius * vec4(fragOffset, 0.0, 0.0);

    gl_Position = ubo.projectionMatrix * positionInCameraSpace;
}
