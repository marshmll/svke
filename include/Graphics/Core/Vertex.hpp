#pragma once

#ifndef GLM_FORCE_RADIANS
#define GLM_FORCE_RADIANS
#endif
#ifndef GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#endif
#include <glm/glm.hpp>

#ifndef GLFW_INCLUDE_VULKAN
#define GLFW_INCLUDE_VULKAN
#endif

#include <GLFW/glfw3.h>

#include <vector>

namespace fl
{
struct Vertex;

typedef std::vector<Vertex> VertexArray;

struct Vertex
{
    glm::vec2 position;

    inline static std::vector<VkVertexInputBindingDescription> getBindingDescriptions()
    {
        std::vector<VkVertexInputBindingDescription> binding_descriptions(1);

        binding_descriptions[0].binding = 0;
        binding_descriptions[0].stride = sizeof(Vertex);
        binding_descriptions[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

        return std::move(binding_descriptions);
    }

    inline static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions()
    {
        std::vector<VkVertexInputAttributeDescription> attribute_descriptions(1);

        attribute_descriptions[0].binding = 0;
        attribute_descriptions[0].location = 0;
        attribute_descriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
        attribute_descriptions[0].offset = 0;

        return std::move(attribute_descriptions);
    }
};
} // namespace fl
