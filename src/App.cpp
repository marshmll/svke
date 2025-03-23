#include "App.hpp"

fl::App::App()
{

    window = std::make_unique<Window>(846, 468, "Flare");
    device = std::make_unique<Device>(*window);
    swapchain = std::make_unique<Swapchain>(*device, *window);
    loadModels();
    createPipelineLayout();
    createPipeline();
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

void fl::App::loadModels()
{
    VertexArray vertices = VertexArray{{Vertex{{0.0f, -0.5f}}, Vertex{{0.0f, 0.5f}}, Vertex{{-0.5f, 0.5f}}}};

    model = std::make_unique<Model>(*device);

    model->loadFromData(vertices);
}

void fl::App::createPipelineLayout()
{
    VkPipelineLayoutCreateInfo pipeline_layout_info = {};
    pipeline_layout_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipeline_layout_info.setLayoutCount = 0;
    pipeline_layout_info.pSetLayouts = nullptr;
    pipeline_layout_info.pushConstantRangeCount = 0;
    pipeline_layout_info.pPushConstantRanges = nullptr;

    if (vkCreatePipelineLayout(device->getLogicalDevice(), &pipeline_layout_info, nullptr, &pipelineLayout) !=
        VK_SUCCESS)
        throw std::runtime_error("fl::App::createPipelineLayout: FAILED TO CREATE PIPELINE LAYOUT");
}

void fl::App::createPipeline()
{
    Pipeline::Config pipeline_config = Pipeline::defaultPipelineConfig(swapchain->getWidth(), swapchain->getHeight());
    pipeline_config.renderPass = swapchain->getRenderPass();
    pipeline_config.pipelineLayout = pipelineLayout;

    pipeline = std::make_unique<Pipeline>(*device, "shaders/vertex.spv", "shaders/fragment.spv", pipeline_config);
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

    for (int i = 0; i < commandBuffers.size(); ++i)
    {
        VkCommandBufferBeginInfo begin = {};
        begin.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

        if (vkBeginCommandBuffer(commandBuffers[i], &begin) != VK_SUCCESS)
            throw std::runtime_error("fl::App::createCommandBuffers: FAILED TO BEGIN COMMAND BUFFERS");

        VkRenderPassBeginInfo render_pass_begin = {};
        render_pass_begin.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        render_pass_begin.renderPass = swapchain->getRenderPass();
        render_pass_begin.framebuffer = swapchain->getFramebuffer(i);
        render_pass_begin.renderArea.offset = {0, 0};
        render_pass_begin.renderArea.extent = swapchain->getExtent();

        std::array<VkClearValue, 2> clear_values;
        clear_values[0].color = {.1f, .1f, .1f, 1.f};
        clear_values[1].depthStencil = {1.f, 0};

        render_pass_begin.clearValueCount = static_cast<uint32_t>(clear_values.size());
        render_pass_begin.pClearValues = clear_values.data();

        vkCmdBeginRenderPass(commandBuffers[i], &render_pass_begin, VK_SUBPASS_CONTENTS_INLINE);

        pipeline->bind(commandBuffers[i]);

        model->bind(commandBuffers[i]);
        model->draw(commandBuffers[i]);

        vkCmdEndRenderPass(commandBuffers[i]);

        if (vkEndCommandBuffer(commandBuffers[i]) != VK_SUCCESS)
            throw std::runtime_error("fl::App::createCommandBuffers: FAILED TO END COMMAND BUFFER");
    }
}

void fl::App::drawFrame()
{
    uint32_t image_index = 0;
    auto result = swapchain->acquireNextImage(image_index);

    if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
        throw std::runtime_error("fl::App::drawFrame: FAILED TO ACQUIRE SWAPCHAIN IMAGE");

    result = swapchain->submitCommandBuffers(commandBuffers[image_index], image_index);

    if (result != VK_SUCCESS)
        throw std::runtime_error("fl::App::drawFrame: FAILED TO PRESENT SWAPCHAIN IMAGE");
}
