#include "SVKE/Rendering/Systems/PointLightSystem.hpp"

vk::PointLightSystem::PointLightSystem(Device &device, Renderer &renderer, DescriptorSetLayout &global_set_layout)
    : device(device), pipelineLayout(VK_NULL_HANDLE)
{
    loadShaders();
    createPipelineLayout(global_set_layout);
    createPipeline(renderer.getRenderPass());
}

vk::PointLightSystem::~PointLightSystem()
{
    vkDestroyPipelineLayout(device.getLogicalDevice(), pipelineLayout, nullptr);
}

void vk::PointLightSystem::render(const FrameInfo &frame_info)
{
    pipeline->bind(frame_info.commandBuffer);

    vkCmdBindDescriptorSets(frame_info.commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1,
                            &frame_info.globalDescriptorSet, 0, nullptr);

    vkCmdDraw(frame_info.commandBuffer, 6, 1, 0, 0);
}

void vk::PointLightSystem::loadShaders()
{
    vertShader = std::make_unique<Shader>(device, "assets/shaders/point_light_system.vert.spv");
    fragShader = std::make_unique<Shader>(device, "assets/shaders/point_light_system.frag.spv");
}

void vk::PointLightSystem::createPipelineLayout(DescriptorSetLayout &global_set_layout)
{
    // VkPushConstantRange push_constant_range = {};
    // push_constant_range.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
    // push_constant_range.offset = 0;
    // push_constant_range.size = sizeof(PushConstantData);

    std::vector<VkDescriptorSetLayout> global_set_layouts{global_set_layout.getDescriptorSetLayout()};

    VkPipelineLayoutCreateInfo pipeline_layout_info = {};
    pipeline_layout_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipeline_layout_info.setLayoutCount = static_cast<uint32_t>(global_set_layouts.size());
    pipeline_layout_info.pSetLayouts = global_set_layouts.data();
    pipeline_layout_info.pushConstantRangeCount = 0;
    pipeline_layout_info.pPushConstantRanges = nullptr;

    if (vkCreatePipelineLayout(device.getLogicalDevice(), &pipeline_layout_info, nullptr, &pipelineLayout) !=
        VK_SUCCESS)
        throw std::runtime_error("vk::PointLightSystem::createPipelineLayout: FAILED TO CREATE PIPELINE LAYOUT");
}

void vk::PointLightSystem::createPipeline(VkRenderPass render_pass)
{
    assert(pipelineLayout != VK_NULL_HANDLE && "CANNOT CREATE PIPELINE BEFORE PIPELINE LAYOUT");

    Pipeline::Config pipeline_config = {};
    Pipeline::defaultPipelineConfig(pipeline_config);

    pipeline_config.attributeDescriptions.clear();
    pipeline_config.bindingDescriptions.clear();
    pipeline_config.renderPass = render_pass;
    pipeline_config.pipelineLayout = pipelineLayout;

    pipeline = std::make_unique<Pipeline>(device, *vertShader, *fragShader, pipeline_config);
}
