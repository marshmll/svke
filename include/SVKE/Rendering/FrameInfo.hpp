#pragma once

#ifndef GLFW_INCLUDE_VULKAN
#define GLFW_INCLUDE_VULKAN
#endif

#include <GLFW/glfw3.h>

#include "SVKE/Rendering/Camera.hpp"
#include "SVKE/Rendering/Resources/Object.hpp"

namespace vk
{
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
