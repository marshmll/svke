#include "SVKE/Core/System/Pipeline.hpp"

vk::Pipeline::Pipeline(Device &device, const std::string &vert_path, const std::string &frag_path) : device(device)
{
    Config config = {};
    defaultPipelineConfig(config);

    Shader vert_shader(device, vert_path);
    Shader frag_shader(device, frag_path);

    createGraphicsPipeline(config, vert_shader, frag_shader);
}

vk::Pipeline::Pipeline(Device &device, const std::string &vert_path, const std::string &frag_path, const Config &config)
    : device(device)
{
    Shader vert_shader(device, vert_path);
    Shader frag_shader(device, frag_path);

    createGraphicsPipeline(config, vert_shader, frag_shader);
}

vk::Pipeline::Pipeline(Device &device, Shader &vert_shader, Shader &frag_shader) : device(device)
{
    Config config = {};
    defaultPipelineConfig(config);

    createGraphicsPipeline(config, vert_shader, frag_shader);
}

vk::Pipeline::Pipeline(Device &device, Shader &vert_shader, Shader &frag_shader, const Config &config) : device(device)
{
    createGraphicsPipeline(config, vert_shader, frag_shader);
}

vk::Pipeline::~Pipeline()
{
    vkDeviceWaitIdle(device.getLogicalDevice());
    vkDestroyPipeline(device.getLogicalDevice(), graphicsPipeline, nullptr);
}

void vk::Pipeline::bind(VkCommandBuffer &command_buffer)
{
    vkCmdBindPipeline(command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline);
}

void vk::Pipeline::defaultPipelineConfig(Config &config)
{
    /* VIEWPORT -------------------------------------------------------------------------------------------- */
    config.viewportInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    config.viewportInfo.viewportCount = 1;
    config.viewportInfo.pViewports = nullptr;
    config.viewportInfo.scissorCount = 1;
    config.viewportInfo.pScissors = nullptr;

    /* INPUT ASSEMBLY -------------------------------------------------------------------------------------- */
    config.inputAssemblyInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    config.inputAssemblyInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    config.inputAssemblyInfo.primitiveRestartEnable = VK_FALSE;

    /* RASTERIZATION --------------------------------------------------------------------------------------- */
    config.rasterizationInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    config.rasterizationInfo.depthClampEnable = VK_FALSE;
    config.rasterizationInfo.rasterizerDiscardEnable = VK_FALSE;
    config.rasterizationInfo.polygonMode = VK_POLYGON_MODE_FILL;
    config.rasterizationInfo.lineWidth = 1.f;
    config.rasterizationInfo.cullMode = VK_CULL_MODE_NONE;
    config.rasterizationInfo.frontFace = VK_FRONT_FACE_CLOCKWISE;
    config.rasterizationInfo.depthBiasEnable = VK_FALSE;
    config.rasterizationInfo.depthBiasConstantFactor = 0.f;
    config.rasterizationInfo.depthBiasClamp = 0.f;
    config.rasterizationInfo.depthBiasSlopeFactor = 0.f;

    /* MULTISAMPLE ---------------------------------------------------------------------------------------- */
    config.multisampleInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    config.multisampleInfo.sampleShadingEnable = VK_FALSE;
    config.multisampleInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    config.multisampleInfo.minSampleShading = 1.f;
    config.multisampleInfo.pSampleMask = nullptr;
    config.multisampleInfo.alphaToCoverageEnable = VK_FALSE;
    config.multisampleInfo.alphaToOneEnable = VK_FALSE;

    /* COLOR BLENDING ATTACHMENT --------------------------------------------------------------------------- */
    config.colorBlendAttachment.colorWriteMask =
        VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    config.colorBlendAttachment.blendEnable = VK_FALSE;
    config.colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
    config.colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
    config.colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
    config.colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
    config.colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
    config.colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;

    /* COLOR BLENDING -------------------------------------------------------------------------------------- */
    config.colorBlendInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    config.colorBlendInfo.logicOpEnable = VK_FALSE;
    config.colorBlendInfo.logicOp = VK_LOGIC_OP_COPY;
    config.colorBlendInfo.attachmentCount = 1;
    config.colorBlendInfo.pAttachments = &config.colorBlendAttachment;
    config.colorBlendInfo.blendConstants[0] = 0.f;
    config.colorBlendInfo.blendConstants[1] = 0.f;
    config.colorBlendInfo.blendConstants[2] = 0.f;
    config.colorBlendInfo.blendConstants[3] = 0.f;

    /* DEPTH STENCIL --------------------------------------------------------------------------------------- */
    config.depthStencilInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    config.depthStencilInfo.depthTestEnable = VK_TRUE;
    config.depthStencilInfo.depthWriteEnable = VK_TRUE;
    config.depthStencilInfo.depthCompareOp = VK_COMPARE_OP_LESS;
    config.depthStencilInfo.depthBoundsTestEnable = VK_FALSE;
    config.depthStencilInfo.minDepthBounds = 0.f;
    config.depthStencilInfo.maxDepthBounds = 1.f;
    config.depthStencilInfo.stencilTestEnable = VK_FALSE;
    config.depthStencilInfo.front = {};
    config.depthStencilInfo.back = {};

    /* DYNAMIC STATES -------------------------------------------------------------------------------------- */
    config.dynamicStatesEnable = {VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR};
    config.dynamicStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    config.dynamicStateInfo.dynamicStateCount = static_cast<uint32_t>(config.dynamicStatesEnable.size());
    config.dynamicStateInfo.pDynamicStates = config.dynamicStatesEnable.data();
    config.dynamicStateInfo.flags = 0;
}

void vk::Pipeline::createGraphicsPipeline(const Config &config, Shader &vert_shader, Shader &frag_shader)
{
    assert(config.pipelineLayout != VK_NULL_HANDLE && "PIPELINE LAYOUT WAS NOT PROVIDED OR IS A VK_NULL_HANDLE");
    assert(config.renderPass != VK_NULL_HANDLE && "PIPELINE RENDER PASS WAS NOT PROVIDED OR IS A VK_NULL_HANDLE");

    VkPipelineShaderStageCreateInfo shader_stages[2];
    shader_stages[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    shader_stages[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
    shader_stages[0].module = vert_shader.getModule();
    shader_stages[0].pName = "main";
    shader_stages[0].flags = 0;
    shader_stages[0].pNext = nullptr;
    shader_stages[0].pSpecializationInfo = nullptr;

    shader_stages[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    shader_stages[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    shader_stages[1].module = frag_shader.getModule();
    shader_stages[1].pName = "main";
    shader_stages[1].flags = 0;
    shader_stages[1].pNext = nullptr;
    shader_stages[1].pSpecializationInfo = nullptr;

    VkPipelineVertexInputStateCreateInfo vertex_input_info = {};
    auto attribute_descriptions = Vertex::getAttributeDescriptions();
    auto binding_descriptions = Vertex::getBindingDescriptions();
    vertex_input_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertex_input_info.vertexAttributeDescriptionCount = static_cast<uint32_t>(attribute_descriptions.size());
    vertex_input_info.vertexBindingDescriptionCount = static_cast<uint32_t>(binding_descriptions.size());
    vertex_input_info.pVertexAttributeDescriptions = attribute_descriptions.data();
    vertex_input_info.pVertexBindingDescriptions = binding_descriptions.data();

    VkGraphicsPipelineCreateInfo pipeline_info = {};
    pipeline_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipeline_info.stageCount = 2;
    pipeline_info.pStages = shader_stages;
    pipeline_info.pVertexInputState = &vertex_input_info;
    pipeline_info.pInputAssemblyState = &config.inputAssemblyInfo;
    pipeline_info.pViewportState = &config.viewportInfo;
    pipeline_info.pRasterizationState = &config.rasterizationInfo;
    pipeline_info.pMultisampleState = &config.multisampleInfo;
    pipeline_info.pColorBlendState = &config.colorBlendInfo;
    pipeline_info.pDepthStencilState = &config.depthStencilInfo;
    pipeline_info.pDynamicState = &config.dynamicStateInfo;

    pipeline_info.layout = config.pipelineLayout;
    pipeline_info.renderPass = config.renderPass;
    pipeline_info.subpass = config.subpass;

    pipeline_info.basePipelineIndex = -1;
    pipeline_info.basePipelineHandle = VK_NULL_HANDLE;

    if (vkCreateGraphicsPipelines(device.getLogicalDevice(), VK_NULL_HANDLE, 1, &pipeline_info, nullptr,
                                  &graphicsPipeline) != VK_SUCCESS)
        throw std::runtime_error("vk::Pipeline::defaultPipelineConfig: FAILED TO CREATE GRAPHICS PIPELINE");
}
