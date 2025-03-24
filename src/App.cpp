#include "App.hpp"

fl::App::App()
{
    window = std::make_unique<Window>(600, 600, "Flare");
    device = std::make_unique<Device>(*window);
    swapchain = std::make_unique<Swapchain>(*device, *window);

    vertShader = std::make_unique<Shader>(*device, "shaders/vertex.spv");
    fragShader = std::make_unique<Shader>(*device, "shaders/fragment.spv");

    loadDrawables();
    createPipelineLayout();
    recreateSwapchain();
    createCommandBuffers();
}

fl::App::~App()
{
    vkDestroyPipelineLayout(device->getLogicalDevice(), pipelineLayout, nullptr);
}

void fl::App::run()
{
    while (!window->shouldClose())
    {
        window->pollEvents();
        drawFrame();
    }
}

void fl::App::loadDrawables()
{
    VertexArray vertices =
        VertexArray{{Vertex{{0.0f, -0.5f}, {1.0f, 0.0f, 0.0f}}, Vertex{{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
                     Vertex{{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}}}};

    auto model = std::make_shared<Model>(*device);
    model->loadFromData(vertices);

    Drawable triangle;
    triangle.setModel(model);
    triangle.setColor({1.f, 0.f, 0.f});
    triangle.translate({0.f, 0.f});
    triangle.setScale({2.f, 0.5f});
    triangle.rotate(45.f);

    drawables.push_back(std::move(triangle));
}

void fl::App::createPipelineLayout()
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

    if (vkCreatePipelineLayout(device->getLogicalDevice(), &pipeline_layout_info, nullptr, &pipelineLayout) !=
        VK_SUCCESS)
        throw std::runtime_error("fl::App::createPipelineLayout: FAILED TO CREATE PIPELINE LAYOUT");
}

void fl::App::createPipeline()
{
    Pipeline::Config pipeline_config = {};
    Pipeline::defaultPipelineConfig(pipeline_config);

    pipeline_config.renderPass = swapchain->getRenderPass();
    pipeline_config.pipelineLayout = pipelineLayout;

    pipeline = std::make_unique<Pipeline>(*device, *vertShader, *fragShader, pipeline_config);
}

void fl::App::createCommandBuffers()
{
    commandBuffers.resize(swapchain->getImageCount());

    VkCommandBufferAllocateInfo alloc_info = {};
    alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    alloc_info.commandPool = device->getCommandPool();
    alloc_info.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());

    if (vkAllocateCommandBuffers(device->getLogicalDevice(), &alloc_info, commandBuffers.data()) != VK_SUCCESS)
        throw std::runtime_error("fl::App::createCommandBuffers: FAILED TO ALLOCATE COMMAND BUFFERS");
}

void fl::App::freeCommandBuffers()
{
    vkFreeCommandBuffers(device->getLogicalDevice(), device->getCommandPool(),
                         static_cast<uint32_t>(commandBuffers.size()), commandBuffers.data());

    commandBuffers.clear();
}

void fl::App::drawFrame()
{
    uint32_t image_index = 0;
    auto result = swapchain->acquireNextImage(image_index);

    if (result == VK_ERROR_OUT_OF_DATE_KHR)
    {
        recreateSwapchain();
        return;
    }

    if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
        throw std::runtime_error("fl::App::drawFrame: FAILED TO ACQUIRE SWAPCHAIN IMAGE");

    recordCommandBuffer(image_index);
    result = swapchain->submitCommandBuffers(commandBuffers[image_index], image_index);

    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || window->wasResized())
    {
        window->setResized(false);
        recreateSwapchain();

        return;
    }

    if (result != VK_SUCCESS)
        throw std::runtime_error("fl::App::drawFrame: FAILED TO PRESENT SWAPCHAIN IMAGE");
}

void fl::App::drawDrawables(VkCommandBuffer &command_buffer)
{
    pipeline->bind(command_buffer);

    for (auto &drawable : drawables)
    {
        PushConstantData push = {};
        push.offset = drawable.getTransform2d().translation;
        push.color = drawable.getColor();
        push.transform = drawable.transform();

        vkCmdPushConstants(command_buffer, pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0,
                           sizeof(PushConstantData), &push);

        drawable.bind(command_buffer);
        drawable.draw(command_buffer);
    }
}

void fl::App::recreateSwapchain()
{
    auto extent = window->getExtent();

    while (extent.width == 0 || extent.height == 0)
    {
        extent = window->getExtent();
        glfwWaitEvents();
    }

    vkDeviceWaitIdle(device->getLogicalDevice());

    if (!swapchain)
    {
        swapchain = std::make_unique<Swapchain>(*device, *window);
    }
    else
    {
        std::shared_ptr<Swapchain> old_swapchain = std::move(swapchain);
        swapchain = std::make_unique<Swapchain>(*device, *window, old_swapchain);

        if (swapchain->getImageCount() != commandBuffers.size())
        {
            if (commandBuffers.size() > 0)
                freeCommandBuffers();

            createCommandBuffers();
        }
    }

    createPipeline();
}

void fl::App::recordCommandBuffer(uint32_t image_index)
{
    /* BEGIN COMMAND BUFFER --------------------------------------------------------------------------------- */

    VkCommandBufferBeginInfo begin = {};
    begin.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

    if (vkBeginCommandBuffer(commandBuffers[image_index], &begin) != VK_SUCCESS)
        throw std::runtime_error("fl::App::recordCommandBuffer: FAILED TO RECORD COMMAND BUFFER");

    /* RENDER PASS BEGIN ------------------------------------------------------------------------------------ */

    VkRenderPassBeginInfo render_pass_begin = {};
    render_pass_begin.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    render_pass_begin.renderPass = swapchain->getRenderPass();
    render_pass_begin.framebuffer = swapchain->getFramebuffer(image_index);
    render_pass_begin.renderArea.offset = {0, 0};
    render_pass_begin.renderArea.extent = swapchain->getExtent();

    /* CLEAR COLOR AND DEPTH -------------------------------------------------------------------------------- */

    std::array<VkClearValue, 2> clear_values;
    clear_values[0].color = {.1f, .1f, .1f, 1.f};
    clear_values[1].depthStencil = {1.f, 0};

    render_pass_begin.clearValueCount = static_cast<uint32_t>(clear_values.size());
    render_pass_begin.pClearValues = clear_values.data();

    vkCmdBeginRenderPass(commandBuffers[image_index], &render_pass_begin, VK_SUBPASS_CONTENTS_INLINE);

    /* VIEWPORT AND SCISSOR --------------------------------------------------------------------------------- */

    VkViewport viewport = {};
    viewport.x = 0;
    viewport.y = 0;
    viewport.width = static_cast<float>(swapchain->getWidth());
    viewport.height = static_cast<float>(swapchain->getHeight());
    viewport.minDepth = 0.f;
    viewport.maxDepth = 1.f;

    VkRect2D scissor = {};
    scissor.offset = {0, 0};
    scissor.extent = swapchain->getExtent();

    vkCmdSetViewport(commandBuffers[image_index], 0, 1, &viewport);
    vkCmdSetScissor(commandBuffers[image_index], 0, 1, &scissor);

    drawDrawables(commandBuffers[image_index]);

    /* END RENDER PASS -------------------------------------------------------------------------------------- */

    vkCmdEndRenderPass(commandBuffers[image_index]);

    /* END COMMAND BUFFER ----------------------------------------------------------------------------------- */

    if (vkEndCommandBuffer(commandBuffers[image_index]) != VK_SUCCESS)
        throw std::runtime_error("fl::App::recordCommandBuffer: FAILED TO END COMMAND BUFFER");
}
