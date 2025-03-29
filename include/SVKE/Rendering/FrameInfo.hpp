#pragma once

#ifndef GLFW_INCLUDE_VULKAN
#define GLFW_INCLUDE_VULKAN
#endif

#include <GLFW/glfw3.h>

#include "SVKE/Rendering/Camera.hpp"

namespace vk
{
struct FrameInfo
{
    int frameIndex;
    float dt;
    VkCommandBuffer &commandBuffer;
    Camera &camera;
    VkDescriptorSet &globalDescriptorSet;
};
} // namespace vk
