#version 450

/* INPUT */
layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec3 inNormal;
layout(location = 3) in vec2 inUv;

layout(push_constant) uniform Push
{
    mat4 transform;
    vec3 color;
}
push;

/* OUTPUT */
layout(location = 0) out vec3 fragColor;

void main()
{
    gl_Position = push.transform * vec4(inPosition, 1.0);
    fragColor = inColor;
}
