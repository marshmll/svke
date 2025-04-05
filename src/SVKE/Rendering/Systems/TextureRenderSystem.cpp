#include "SVKE/Rendering/Systems/TextureRenderSystem.hpp"

vk::TextureRenderSystem::TextureRenderSystem(Device &device, Renderer &renderer,
                                             std::vector<VkDescriptorSetLayout> &set_layouts)
    : device(device), pipelineLayout(VK_NULL_HANDLE)
{
    loadShaders();
    createPipelineLayout(set_layouts);
    createPipeline(renderer.getRenderPass());
}

vk::TextureRenderSystem::~TextureRenderSystem()
{
    vkDestroyPipelineLayout(device.getLogicalDevice(), pipelineLayout, nullptr);
}

void vk::TextureRenderSystem::render(const FrameInfo &frame_info)
{
    pipeline->bind(frame_info.commandBuffer);

    vkCmdBindDescriptorSets(frame_info.commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1,
                            &frame_info.globalDescriptorSet, 0, nullptr);

    for (auto &[_, object] : frame_info.objects)
    {
        if (!object.getTextureImage() || frame_info.objectDescriptorSets.size() == 0)
            continue;

        vkCmdBindDescriptorSets(frame_info.commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 1, 1,
                                &frame_info.objectDescriptorSets[object.getId()], 0, nullptr);

        PushConstantData push = {};
        push.modelMatrix = object.transform();
        push.normalMatrix = object.normalMatrix();

        vkCmdPushConstants(frame_info.commandBuffer, pipelineLayout,
                           VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(PushConstantData),
                           &push);

        object.bind(frame_info.commandBuffer);
        object.draw(frame_info.commandBuffer);
    }
}

void vk::TextureRenderSystem::loadShaders()
{
    vertShader = std::make_unique<Shader>(device, "assets/shaders/texture_render_system.vert.spv");
    fragShader = std::make_unique<Shader>(device, "assets/shaders/texture_render_system.frag.spv");
}

void vk::TextureRenderSystem::createPipelineLayout(std::vector<VkDescriptorSetLayout> &set_layouts)
{
    VkPushConstantRange push_constant_range = {};
    push_constant_range.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
    push_constant_range.offset = 0;
    push_constant_range.size = sizeof(PushConstantData);

    VkPipelineLayoutCreateInfo pipeline_layout_info = {};
    pipeline_layout_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipeline_layout_info.setLayoutCount = static_cast<uint32_t>(set_layouts.size());
    pipeline_layout_info.pSetLayouts = set_layouts.data();
    pipeline_layout_info.pushConstantRangeCount = 1;
    pipeline_layout_info.pPushConstantRanges = &push_constant_range;

    if (vkCreatePipelineLayout(device.getLogicalDevice(), &pipeline_layout_info, nullptr, &pipelineLayout) !=
        VK_SUCCESS)
        throw std::runtime_error("vk::TextureRenderSystem::createPipelineLayout: FAILED TO CREATE PIPELINE LAYOUT");
}

void vk::TextureRenderSystem::createPipeline(VkRenderPass render_pass)
{
    assert(pipelineLayout != VK_NULL_HANDLE && "CANNOT CREATE PIPELINE BEFORE PIPELINE LAYOUT");

    Pipeline::Config pipeline_config = {};
    Pipeline::defaultPipelineConfig(pipeline_config);

    pipeline_config.renderPass = render_pass;
    pipeline_config.pipelineLayout = pipelineLayout;
    pipeline_config.multisampleInfo.rasterizationSamples = device.getCurrentMsaaSamples();
    pipeline_config.rasterizationInfo.cullMode = VK_CULL_MODE_BACK_BIT;
    pipeline_config.rasterizationInfo.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;

    // Optional: Shader antialiasing, smooths inner parts of shapes. Might cost some performance
    pipeline_config.multisampleInfo.sampleShadingEnable = VK_TRUE;
    pipeline_config.multisampleInfo.minSampleShading = .2f;

    pipeline = std::make_unique<Pipeline>(device, *vertShader, *fragShader, pipeline_config);
}
