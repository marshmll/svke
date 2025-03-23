#pragma once

#include "System/Core/Device.hpp"
#include "System/Shader/Shader.hpp"
#include "Graphics/Core/Vertex.hpp"

#include <string>
#include <fstream>
#include <vector>
#include <cstdint>
#include <iostream>
#include <memory>

namespace fl
{
class Pipeline
{
  public:
    struct Config
    {
        VkViewport viewport;
        VkRect2D scissor;
        VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo;
        VkPipelineRasterizationStateCreateInfo rasterizationInfo;
        VkPipelineMultisampleStateCreateInfo multisampleInfo;
        VkPipelineColorBlendAttachmentState colorBlendAttachment;
        VkPipelineColorBlendStateCreateInfo colorBlendInfo;
        VkPipelineDepthStencilStateCreateInfo depthStencilInfo;
        VkPipelineLayout pipelineLayout = VK_NULL_HANDLE;
        VkRenderPass renderPass = VK_NULL_HANDLE;
        uint32_t subpass = 0;
    };

    Pipeline(Device &device, const std::string &vert_path, const std::string &frag_path);
    Pipeline(Device &device, const std::string &vert_path, const std::string &frag_path, const Config &config);

    Pipeline(const Pipeline &) = delete;
    Pipeline &operator=(const Pipeline &) = delete;

    ~Pipeline();

    void bind(VkCommandBuffer &command_buffer);

    static const Config defaultPipelineConfig(const uint32_t width, const uint32_t height);

  private:
    Device &device;
    Config config;
    VkPipeline graphicsPipeline;

    void createGraphicsPipeline(const Config &config, const std::string &vert_path, const std::string &frag_path);
};

} // namespace fl
