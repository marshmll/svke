#version 450

/* INPUT */
layout(push_constant) uniform Push
{
    mat4 transform;
    vec3 color;
}
push;

layout(location = 0) in vec3 fragColor;

/* OUTPUT */
layout(location = 0) out vec4 outColor;

void main()
{
    outColor = vec4(fragColor, 1.0);
}
