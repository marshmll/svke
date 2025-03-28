#include "SVKE/Rendering/Systems/RenderSystem.hpp"

vk::RenderSystem::RenderSystem(Device &device, Renderer &renderer) : device(device), pipelineLayout(VK_NULL_HANDLE)
{
    loadShaders();
    createPipelineLayout();
    createPipeline(renderer.getRenderPass());
}

vk::RenderSystem::~RenderSystem()
{
    vkDestroyPipelineLayout(device.getLogicalDevice(), pipelineLayout, nullptr);
}

void vk::RenderSystem::render(VkCommandBuffer &command_buffer, std::vector<Object> &objects, const Camera &camera)
{
    pipeline->bind(command_buffer);

    auto projection_view = camera.getProjectionMatrix() * camera.getViewMatrix();

    for (auto &object : objects)
    {
        PushConstantData push = {};
        push.color = object.getColor().toVec3();
        push.transform = projection_view * object.transform();

        vkCmdPushConstants(command_buffer, pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0,
                           sizeof(PushConstantData), &push);

        object.bind(command_buffer);
        object.draw(command_buffer);
    }
}

void vk::RenderSystem::loadShaders()
{
    vertShader = std::make_unique<Shader>(device, "assets/shaders/vertex.spv");
    fragShader = std::make_unique<Shader>(device, "assets/shaders/fragment.spv");
}

void vk::RenderSystem::createPipelineLayout()
{
    VkPushConstantRange push_constant_range = {};
    push_constant_range.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
    push_constant_range.offset = 0;
    push_constant_range.size = sizeof(PushConstantData);

    VkPipelineLayoutCreateInfo pipeline_layout_info = {};
    pipeline_layout_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipeline_layout_info.setLayoutCount = 0;
    pipeline_layout_info.pSetLayouts = nullptr;
    pipeline_layout_info.pushConstantRangeCount = 1;
    pipeline_layout_info.pPushConstantRanges = &push_constant_range;

    if (vkCreatePipelineLayout(device.getLogicalDevice(), &pipeline_layout_info, nullptr, &pipelineLayout) !=
        VK_SUCCESS)
        throw std::runtime_error("vk::RenderSystem::createPipelineLayout: FAILED TO CREATE PIPELINE LAYOUT");
}

void vk::RenderSystem::createPipeline(VkRenderPass render_pass)
{
    assert(pipelineLayout != VK_NULL_HANDLE && "CANNOT CREATE PIPELINE BEFORE PIPELINE LAYOUT");

    Pipeline::Config pipeline_config = {};
    Pipeline::defaultPipelineConfig(pipeline_config);

    pipeline_config.renderPass = render_pass;
    pipeline_config.pipelineLayout = pipelineLayout;

    pipeline = std::make_unique<Pipeline>(device, *vertShader, *fragShader, pipeline_config);
}
