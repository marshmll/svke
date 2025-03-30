#pragma once

#include "SVKE/Core/System/Device.hpp"
#include "SVKE/Core/Graphics/Shader.hpp"
#include "SVKE/Core/Graphics/Vertex.hpp"

#include <string>
#include <fstream>
#include <vector>
#include <cstdint>
#include <iostream>
#include <memory>

namespace vk
{
class Pipeline
{
  public:
    struct Config
    {
        Config() = default;
        Config(const Config &) = delete;
        Config &operator=(const Config &) = delete;

        std::vector<VkVertexInputAttributeDescription> attributeDescriptions;
        std::vector<VkVertexInputBindingDescription> bindingDescriptions;

        VkPipelineViewportStateCreateInfo viewportInfo;
        VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo;
        VkPipelineRasterizationStateCreateInfo rasterizationInfo;
        VkPipelineMultisampleStateCreateInfo multisampleInfo;
        VkPipelineColorBlendAttachmentState colorBlendAttachment;
        VkPipelineColorBlendStateCreateInfo colorBlendInfo;
        VkPipelineDepthStencilStateCreateInfo depthStencilInfo;
        std::vector<VkDynamicState> dynamicStatesEnable;
        VkPipelineDynamicStateCreateInfo dynamicStateInfo;
        VkPipelineLayout pipelineLayout = VK_NULL_HANDLE;
        VkRenderPass renderPass = VK_NULL_HANDLE;
        uint32_t subpass = 0;
    };

    Pipeline(Device &device, const std::string &vert_path, const std::string &frag_path);
    Pipeline(Device &device, const std::string &vert_path, const std::string &frag_path, const Config &config);

    Pipeline(Device &device, Shader &vert_shader, Shader &frag_shader);
    Pipeline(Device &device, Shader &vert_shader, Shader &frag_shader, const Config &config);

    Pipeline(const Pipeline &) = delete;
    Pipeline &operator=(const Pipeline &) = delete;

    ~Pipeline();

    void bind(VkCommandBuffer &command_buffer);

    static void defaultPipelineConfig(Config &config);

    static void enableAlphaBlending(Config &config);

  private:
    Device &device;
    VkPipeline graphicsPipeline;

    void createGraphicsPipeline(const Config &config, const std::string &vert_path, const std::string &frag_path);
    void createGraphicsPipeline(const Config &config, Shader &vert_shader, Shader &frag_shader);
};

} // namespace vk
