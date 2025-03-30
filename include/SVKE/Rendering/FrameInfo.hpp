#pragma once

#ifndef GLFW_INCLUDE_VULKAN
#define GLFW_INCLUDE_VULKAN
#endif

#include <GLFW/glfw3.h>

#include "SVKE/Rendering/Camera.hpp"
#include "SVKE/Rendering/Resources/Object.hpp"
#include "SVKE/Core/System/Memory/Alignment.hpp"

constexpr int MAX_LIGHTS = 10;

namespace vk
{
struct PointLight
{
    ALIGNAS_VEC3 Vec3f position{};
    ALIGNAS_VEC4 Vec4f color{}; // w component is light intensity
};

struct GlobalUBO
{
    ALIGNAS_MAT4 Mat4f projectionMatrix{1.f};
    ALIGNAS_MAT4 Mat4f viewMatrix{1.f};
    ALIGNAS_MAT4 Mat4f inverseViewMatrix{1.f};
    ALIGNAS_VEC4 Vec4f ambientLightColor{1.f, 1.f, 1.f, .01f}; // w = intensity
    ALIGNAS_NESTED_UNIFORM PointLight pointLights[MAX_LIGHTS];
    ALIGNAS_SCLR(int) int numLights;
};

struct FrameInfo
{
    int frameIndex;
    float dt;
    VkCommandBuffer &commandBuffer;
    Camera &camera;
    VkDescriptorSet &globalDescriptorSet;
    Object::Map &objects;
};
} // namespace vk
