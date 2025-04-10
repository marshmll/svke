#include "SVKE/Rendering/Systems/Renderer.hpp"

vk::Renderer::Renderer(Device &device, Window &window, const Swapchain::PresentMode &preferred_present_mode,
                       const Color &clear_color)
    : device(device), window(window), preferredPresentMode(preferred_present_mode), clearColor(clear_color),
      currentImageIndex(0), currentFrameIndex(0), frameInProgress(false)
{
    recreateSwapchain();
    createCommandBuffers();
}

vk::Renderer::~Renderer()
{
    freeCommandBuffers();
}

VkCommandBuffer vk::Renderer::beginFrame()
{
    assert(!frameInProgress && "CANNOT BEGIN FRAME WHEN ANOTHER FRAME IS IN PROGRESS");

    auto result = swapchain->acquireNextImage(currentImageIndex);

    if (result == VK_ERROR_OUT_OF_DATE_KHR)
    {
        recreateSwapchain();
        return VK_NULL_HANDLE;
    }

    if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
        throw std::runtime_error("vk::Renderer::beginFrame: FAILED TO ACQUIRE SWAPCHAIN IMAGE");

    frameInProgress = true;
    auto &command_buffer = getCurrentCommandBuffer();

    /* BEGIN COMMAND BUFFER --------------------------------------------------------------------------------- */

    VkCommandBufferBeginInfo begin = {};
    begin.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

    if (vkBeginCommandBuffer(command_buffer, &begin) != VK_SUCCESS)
        throw std::runtime_error("vk::Renderer::beginFrame: FAILED TO BEGIN RECORDING COMMAND BUFFER");

    return command_buffer;
}

void vk::Renderer::endFrame()
{
    assert(frameInProgress && "CANNOT END FRAME WHEN NO FRAME IS IN PROGRESS");

    /* END COMMAND BUFFER ----------------------------------------------------------------------------------- */

    auto &command_buffer = getCurrentCommandBuffer();

    if (vkEndCommandBuffer(command_buffer) != VK_SUCCESS)
        throw std::runtime_error("vk::Renderer::endFrame: FAILED TO END COMMAND BUFFER");

    auto result = swapchain->submitCommandBuffers(command_buffer, currentImageIndex);

    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || window.wasResized())
    {
        window.setResized(false);
        recreateSwapchain();
    }
    else if (result != VK_SUCCESS)
    {
        throw std::runtime_error("vk::Renderer::endFrame: FAILED TO PRESENT SWAPCHAIN IMAGE");
    }

    frameInProgress = false;
    currentFrameIndex = (currentFrameIndex + 1) % Swapchain::MAX_FRAMES_IN_FLIGHT;
}

void vk::Renderer::beginRenderPass(VkCommandBuffer &command_buffer)
{
    assert(frameInProgress && "CANNOT BEGIN RENDER PASS WHEN NO FRAME IS IN PROGRESS");
    assert(command_buffer == getCurrentCommandBuffer() &&
           "CANNOT BEGIN RENDER PASS ON A COMMAND BUFFER FROM A DIFFERENT FRAME");

    /* RENDER PASS BEGIN ------------------------------------------------------------------------------------ */

    VkRenderPassBeginInfo render_pass_begin = {};
    render_pass_begin.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    render_pass_begin.renderPass = swapchain->getRenderPass();
    render_pass_begin.framebuffer = swapchain->getFramebuffer(currentImageIndex);
    render_pass_begin.renderArea.offset = {0, 0};
    render_pass_begin.renderArea.extent = swapchain->getExtent();

    /* CLEAR COLOR AND DEPTH -------------------------------------------------------------------------------- */

    std::array<VkClearValue, 2> clear_values;
    clear_values[0].color = clearColor.toVkClearColorValue();
    clear_values[1].depthStencil = {1.f, 0};

    render_pass_begin.clearValueCount = static_cast<uint32_t>(clear_values.size());
    render_pass_begin.pClearValues = clear_values.data();

    vkCmdBeginRenderPass(command_buffer, &render_pass_begin, VK_SUBPASS_CONTENTS_INLINE);

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

    vkCmdSetViewport(command_buffer, 0, 1, &viewport);
    vkCmdSetScissor(command_buffer, 0, 1, &scissor);
}

void vk::Renderer::endRenderPass(VkCommandBuffer &command_buffer)
{
    assert(frameInProgress && "CANNOT END RENDER PASS WHEN NO FRAME IS IN PROGRESS");
    assert(command_buffer == getCurrentCommandBuffer() &&
           "CANNOT END RENDER PASS ON A COMMAND BUFFER FROM A DIFFERENT FRAME");

    /* END RENDER PASS -------------------------------------------------------------------------------------- */

    vkCmdEndRenderPass(command_buffer);
}

const bool vk::Renderer::isFrameInProgress() const
{
    return frameInProgress;
}

const int vk::Renderer::getCurrentFrameIndex() const
{
    assert(frameInProgress && "CANNOT GET CURRENT FRAME INDEX WHILE NO FRAME IS IN PROGRESS");

    return currentFrameIndex;
}

VkCommandBuffer &vk::Renderer::getCurrentCommandBuffer()
{
    assert(frameInProgress && "CANNOT GET CURRENT BUFFER WHILE NO FRAME IS IN PROGRESS");
    assert(currentFrameIndex < commandBuffers.size() && "CURRENT FRAME INDEX IS OUT OF BOUNDS");

    return commandBuffers.at(currentFrameIndex);
}

VkRenderPass vk::Renderer::getRenderPass()
{
    return swapchain->getRenderPass();
}

const float vk::Renderer::getAspectRatio() const
{
    return swapchain->getExtentAspectRatio();
}

void vk::Renderer::createCommandBuffers()
{
    commandBuffers.resize(Swapchain::MAX_FRAMES_IN_FLIGHT);

    VkCommandBufferAllocateInfo alloc_info = {};
    alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    alloc_info.commandPool = device.getCommandPool();
    alloc_info.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());

    if (vkAllocateCommandBuffers(device.getLogicalDevice(), &alloc_info, commandBuffers.data()) != VK_SUCCESS)
        throw std::runtime_error("vk::Renderer::createCommandBuffers: FAILED TO ALLOCATE COMMAND BUFFERS");
}

void vk::Renderer::freeCommandBuffers()
{
    vkFreeCommandBuffers(device.getLogicalDevice(), device.getCommandPool(),
                         static_cast<uint32_t>(commandBuffers.size()), commandBuffers.data());

    commandBuffers.clear();
}

void vk::Renderer::recreateSwapchain()
{
    auto extent = window.getExtent();

    while (extent.width == 0 || extent.height == 0)
    {
        extent = window.getExtent();
        glfwWaitEvents();
    }

    vkDeviceWaitIdle(device.getLogicalDevice());

    if (!swapchain)
    {
        swapchain = std::make_unique<Swapchain>(device, window, preferredPresentMode);
    }
    else
    {
        std::shared_ptr<Swapchain> old_swapchain = std::move(swapchain);
        swapchain = std::make_unique<Swapchain>(device, window, old_swapchain, preferredPresentMode);

        if (!old_swapchain->compatibleWith(*swapchain))
            throw std::runtime_error("vk::Renderer::recreateSwapchain: SWAPCHAIN IMAGE OR DEPTH FORMAT HAS CHANGED");
    }
}
