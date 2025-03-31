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

void vk::PointLightSystem::update(const FrameInfo &frame_info, GlobalUBO &ubo)
{
    auto rotate_light = Matrix::rotate(Matrix::identityMat4f(), frame_info.dt, {0.f, -1.f, 0.f});

    int light_index = 0;

    for (auto &[_, object] : frame_info.objects)
    {
        if (!object.getPointLightComponent())
            continue;

        assert(light_index < MAX_LIGHTS && "POINT LIGHTS EXCEEDED MAXIMUM SPECIFIED");

        // Update
        object.setTranslation(Vec3f{rotate_light * Vec4f{object.getTranslation(), 1.0}});

        // Copy data to UBO
        ubo.pointLights[light_index].position = object.getTranslation();
        ubo.pointLights[light_index].color =
            Vec4f{object.getColor().toVec3(), object.getPointLightComponent()->lightIntensity};

        ++light_index;
    }

    ubo.numLights = light_index;
}

void vk::PointLightSystem::render(const FrameInfo &frame_info)
{
    // Sort lights
    std::map<float, Object::objid_t> sorted;

    for (auto &[_, object] : frame_info.objects)
    {
        if (!object.getPointLightComponent())
            continue;

        // calculate distance
        Vec3f offset = frame_info.camera.getPosition() - object.getTranslation();
        float dis_squared = Vector::dot(offset, offset);
        sorted[dis_squared] = object.getId();
    }

    pipeline->bind(frame_info.commandBuffer);

    vkCmdBindDescriptorSets(frame_info.commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1,
                            &frame_info.globalDescriptorSet, 0, nullptr);

    for (auto it = sorted.rbegin(); it != sorted.rend(); ++it)
    {
        auto &object = frame_info.objects.at(it->second);

        PointLightPushConstant push = {};
        push.position = object.getTranslation();
        push.color = Vec4f{object.getColor().toVec3(), object.getPointLightComponent()->lightIntensity};
        push.radius = object.getScale().x;

        vkCmdPushConstants(frame_info.commandBuffer, pipelineLayout,
                           VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(PointLightPushConstant),
                           &push);

        vkCmdDraw(frame_info.commandBuffer, 6, 1, 0, 0);
    }
}

void vk::PointLightSystem::loadShaders()
{
    vertShader = std::make_unique<Shader>(device, "assets/shaders/point_light_system.vert.spv");
    fragShader = std::make_unique<Shader>(device, "assets/shaders/point_light_system.frag.spv");
}

void vk::PointLightSystem::createPipelineLayout(DescriptorSetLayout &global_set_layout)
{
    VkPushConstantRange push_constant_range = {};
    push_constant_range.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
    push_constant_range.offset = 0;
    push_constant_range.size = sizeof(PointLightPushConstant);

    std::vector<VkDescriptorSetLayout> global_set_layouts{global_set_layout.getDescriptorSetLayout()};

    VkPipelineLayoutCreateInfo pipeline_layout_info = {};
    pipeline_layout_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipeline_layout_info.setLayoutCount = static_cast<uint32_t>(global_set_layouts.size());
    pipeline_layout_info.pSetLayouts = global_set_layouts.data();
    pipeline_layout_info.pushConstantRangeCount = 1;
    pipeline_layout_info.pPushConstantRanges = &push_constant_range;

    if (vkCreatePipelineLayout(device.getLogicalDevice(), &pipeline_layout_info, nullptr, &pipelineLayout) !=
        VK_SUCCESS)
        throw std::runtime_error("vk::PointLightSystem::createPipelineLayout: FAILED TO CREATE PIPELINE LAYOUT");
}

void vk::PointLightSystem::createPipeline(VkRenderPass render_pass)
{
    assert(pipelineLayout != VK_NULL_HANDLE && "CANNOT CREATE PIPELINE BEFORE PIPELINE LAYOUT");

    Pipeline::Config pipeline_config = {};
    Pipeline::defaultPipelineConfig(pipeline_config);
    Pipeline::enableAlphaBlending(pipeline_config);

    pipeline_config.attributeDescriptions.clear();
    pipeline_config.bindingDescriptions.clear();
    pipeline_config.renderPass = render_pass;
    pipeline_config.pipelineLayout = pipelineLayout;

    pipeline = std::make_unique<Pipeline>(device, *vertShader, *fragShader, pipeline_config);
}
