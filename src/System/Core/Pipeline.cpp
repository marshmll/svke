#include "System/Core/Pipeline.hpp"

fl::Pipeline::Pipeline(Device &device, const std::string &vert_path, const std::string &frag_path) : device(device)
{
    config = defaultPipelineConfig(846, 484);
    createGraphicsPipeline(config, vert_path, frag_path);
}

fl::Pipeline::Pipeline(Device &device, const std::string &vert_path, const std::string &frag_path, const Config &config)
    : device(device), config(config)
{
    createGraphicsPipeline(config, vert_path, frag_path);
}

fl::Pipeline::~Pipeline()
{
    vkDeviceWaitIdle(device.getLogicalDevice());
    vkDestroyPipeline(device.getLogicalDevice(), graphicsPipeline, nullptr);
}

void fl::Pipeline::bind(VkCommandBuffer &command_buffer)
{
    vkCmdBindPipeline(command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline);
}

const fl::Pipeline::Config fl::Pipeline::defaultPipelineConfig(const uint32_t width, const uint32_t height)
{
    Config default_config = {};

    /* VIEWPORT -------------------------------------------------------------------------------------------- */
    default_config.viewport.x = 0;
    default_config.viewport.y = 0;
    default_config.viewport.width = static_cast<float>(width);
    default_config.viewport.height = static_cast<float>(height);
    default_config.viewport.minDepth = 0.f;
    default_config.viewport.maxDepth = 1.f;

    /* SCISSOR --------------------------------------------------------------------------------------------- */
    default_config.scissor.offset = {0, 0};
    default_config.scissor.extent = {width, height};

    /* INPUT ASSEMBLY -------------------------------------------------------------------------------------- */
    default_config.inputAssemblyInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    default_config.inputAssemblyInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    default_config.inputAssemblyInfo.primitiveRestartEnable = VK_FALSE;

    /* RASTERIZATION --------------------------------------------------------------------------------------- */
    default_config.rasterizationInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    default_config.rasterizationInfo.depthClampEnable = VK_FALSE;
    default_config.rasterizationInfo.rasterizerDiscardEnable = VK_FALSE;
    default_config.rasterizationInfo.polygonMode = VK_POLYGON_MODE_FILL;
    default_config.rasterizationInfo.lineWidth = 1.f;
    default_config.rasterizationInfo.cullMode = VK_CULL_MODE_NONE;
    default_config.rasterizationInfo.frontFace = VK_FRONT_FACE_CLOCKWISE;
    default_config.rasterizationInfo.depthBiasEnable = VK_FALSE;
    default_config.rasterizationInfo.depthBiasConstantFactor = 0.f;
    default_config.rasterizationInfo.depthBiasClamp = 0.f;
    default_config.rasterizationInfo.depthBiasSlopeFactor = 0.f;

    /* MULTISAMPLE ---------------------------------------------------------------------------------------- */
    default_config.multisampleInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    default_config.multisampleInfo.sampleShadingEnable = VK_FALSE;
    default_config.multisampleInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    default_config.multisampleInfo.minSampleShading = 1.f;
    default_config.multisampleInfo.pSampleMask = nullptr;
    default_config.multisampleInfo.alphaToCoverageEnable = VK_FALSE;
    default_config.multisampleInfo.alphaToOneEnable = VK_FALSE;

    /* COLOR BLENDING ATTACHMENT --------------------------------------------------------------------------- */
    default_config.colorBlendAttachment.colorWriteMask =
        VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    default_config.colorBlendAttachment.blendEnable = VK_FALSE;
    default_config.colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
    default_config.colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
    default_config.colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
    default_config.colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
    default_config.colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
    default_config.colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;

    /* COLOR BLENDING -------------------------------------------------------------------------------------- */
    default_config.colorBlendInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    default_config.colorBlendInfo.logicOpEnable = VK_FALSE;
    default_config.colorBlendInfo.logicOp = VK_LOGIC_OP_COPY;
    default_config.colorBlendInfo.attachmentCount = 1;
    default_config.colorBlendInfo.pAttachments = &default_config.colorBlendAttachment;
    default_config.colorBlendInfo.blendConstants[0] = 0.f;
    default_config.colorBlendInfo.blendConstants[1] = 0.f;
    default_config.colorBlendInfo.blendConstants[2] = 0.f;
    default_config.colorBlendInfo.blendConstants[3] = 0.f;

    /* DEPTH STENCIL --------------------------------------------------------------------------------------- */
    default_config.depthStencilInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    default_config.depthStencilInfo.depthTestEnable = VK_TRUE;
    default_config.depthStencilInfo.depthWriteEnable = VK_TRUE;
    default_config.depthStencilInfo.depthCompareOp = VK_COMPARE_OP_LESS;
    default_config.depthStencilInfo.depthBoundsTestEnable = VK_FALSE;
    default_config.depthStencilInfo.minDepthBounds = 0.f;
    default_config.depthStencilInfo.maxDepthBounds = 1.f;
    default_config.depthStencilInfo.stencilTestEnable = VK_FALSE;
    default_config.depthStencilInfo.front = {};
    default_config.depthStencilInfo.back = {};

    return default_config;
}

void fl::Pipeline::createGraphicsPipeline(const Config &config, const std::string &vert_path,
                                          const std::string &frag_path)
{
    assert(config.pipelineLayout != VK_NULL_HANDLE && "PIPELINE LAYOUT WAS NOT PROVIDED OR IS A VK_NULL_HANDLE");
    assert(config.renderPass != VK_NULL_HANDLE && "PIPELINE RENDER PASS WAS NOT PROVIDED OR IS A VK_NULL_HANDLE");

    Shader vertShader(device, vert_path);
    Shader fragShader(device, frag_path);

    VkPipelineShaderStageCreateInfo shader_stages[2];
    shader_stages[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    shader_stages[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
    shader_stages[0].module = vertShader.getModule();
    shader_stages[0].pName = "main";
    shader_stages[0].flags = 0;
    shader_stages[0].pNext = nullptr;
    shader_stages[0].pSpecializationInfo = nullptr;

    shader_stages[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    shader_stages[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    shader_stages[1].module = fragShader.getModule();
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

    VkPipelineViewportStateCreateInfo viewport_info = {};
    viewport_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewport_info.viewportCount = 1;
    viewport_info.pViewports = &config.viewport;
    viewport_info.scissorCount = 1;
    viewport_info.pScissors = &config.scissor;

    VkGraphicsPipelineCreateInfo pipeline_info = {};
    pipeline_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipeline_info.stageCount = 2;
    pipeline_info.pStages = shader_stages;
    pipeline_info.pVertexInputState = &vertex_input_info;
    pipeline_info.pInputAssemblyState = &config.inputAssemblyInfo;
    pipeline_info.pViewportState = &viewport_info;
    pipeline_info.pRasterizationState = &config.rasterizationInfo;
    pipeline_info.pMultisampleState = &config.multisampleInfo;
    pipeline_info.pColorBlendState = &config.colorBlendInfo;
    pipeline_info.pDepthStencilState = &config.depthStencilInfo;
    pipeline_info.pDynamicState = nullptr;

    pipeline_info.layout = config.pipelineLayout;
    pipeline_info.renderPass = config.renderPass;
    pipeline_info.subpass = config.subpass;

    pipeline_info.basePipelineIndex = -1;
    pipeline_info.basePipelineHandle = VK_NULL_HANDLE;

    if (vkCreateGraphicsPipelines(device.getLogicalDevice(), VK_NULL_HANDLE, 1, &pipeline_info, nullptr,
                                  &graphicsPipeline) != VK_SUCCESS)
        throw std::runtime_error("fl::Pipeline::defaultPipelineConfig: FAILED TO CREATE GRAPHICS PIPELINE");
}
