#include "SVKE/Core/System/Swapchain.hpp"

vk::Swapchain::Swapchain(Device &device, Window &window, const PresentMode &preferred_present_mode)
    : device(device), window(window), currentFrame(0)
{
    init(preferred_present_mode);
}

vk::Swapchain::Swapchain(Device &device, Window &window, std::shared_ptr<Swapchain> &previous,
                         const PresentMode &preferred_present_mode)
    : device(device), window(window), oldSwapchain(previous), currentFrame(0)
{
    init(preferred_present_mode);

    // Give up ownership
    oldSwapchain = nullptr;
}

vk::Swapchain::~Swapchain()
{
    vkDeviceWaitIdle(device.getLogicalDevice());

    for (auto imageView : imageViews)
    {
        vkDestroyImageView(device.getLogicalDevice(), imageView, nullptr);
    }

    if (swapchain != nullptr)
    {
        vkDestroySwapchainKHR(device.getLogicalDevice(), swapchain, nullptr);
        swapchain = nullptr;
    }

    for (int i = 0; i < depthImages.size(); i++)
    {
        vkDestroyImageView(device.getLogicalDevice(), depthImageViews[i], nullptr);
        vmaDestroyImage(device.getAllocator(), depthImages[i], depthImagesMemory[i]);
    }

    for (auto framebuffer : framebuffers)
    {
        vkDestroyFramebuffer(device.getLogicalDevice(), framebuffer, nullptr);
    }

    vkDestroyRenderPass(device.getLogicalDevice(), renderPass, nullptr);

    // cleanup synchronization objects
    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
    {
        vkDestroySemaphore(device.getLogicalDevice(), renderFinishedSemaphores[i], nullptr);
        vkDestroySemaphore(device.getLogicalDevice(), imageAvailableSemaphores[i], nullptr);
        vkDestroyFence(device.getLogicalDevice(), inFlightFences[i], nullptr);
    }
}

VkResult vk::Swapchain::submitCommandBuffers(const VkCommandBuffer &buffers, uint32_t &image_index)
{
    if (imagesInFlight[image_index] != VK_NULL_HANDLE)
    {
        vkWaitForFences(device.getLogicalDevice(), 1, &imagesInFlight[image_index], VK_TRUE, UINT64_MAX);
    }
    imagesInFlight[image_index] = inFlightFences[currentFrame];

    VkSubmitInfo submit_info = {};
    submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    VkSemaphore wait_semaphores[] = {imageAvailableSemaphores[currentFrame]};
    VkPipelineStageFlags wait_stages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    submit_info.waitSemaphoreCount = 1;
    submit_info.pWaitSemaphores = wait_semaphores;
    submit_info.pWaitDstStageMask = wait_stages;

    submit_info.commandBufferCount = 1;
    submit_info.pCommandBuffers = &buffers;

    VkSemaphore signal_semaphores[] = {renderFinishedSemaphores[currentFrame]};
    submit_info.signalSemaphoreCount = 1;
    submit_info.pSignalSemaphores = signal_semaphores;

    vkResetFences(device.getLogicalDevice(), 1, &inFlightFences[currentFrame]);
    if (vkQueueSubmit(device.getGraphicsQueue(), 1, &submit_info, inFlightFences[currentFrame]) != VK_SUCCESS)
        throw std::runtime_error("vk::Swapchain::submitCommandBuffers: FAILED TO SUBMIT COMMAND BUFFER");

    VkPresentInfoKHR present_info = {};
    present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

    present_info.waitSemaphoreCount = 1;
    present_info.pWaitSemaphores = signal_semaphores;

    VkSwapchainKHR swapchains[] = {swapchain};
    present_info.swapchainCount = 1;
    present_info.pSwapchains = swapchains;

    present_info.pImageIndices = &image_index;

    auto result = vkQueuePresentKHR(device.getPresentQueue(), &present_info);

    currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;

    return result;
}

VkResult vk::Swapchain::acquireNextImage(uint32_t &image_index)
{
    vkWaitForFences(device.getLogicalDevice(), 1, &inFlightFences[currentFrame], VK_TRUE,
                    std::numeric_limits<uint64_t>::max());

    VkResult result = vkAcquireNextImageKHR(device.getLogicalDevice(), swapchain, std::numeric_limits<uint64_t>::max(),
                                            imageAvailableSemaphores[currentFrame], // must be a not signaled semaphore
                                            VK_NULL_HANDLE, &image_index);

    return result;
}

VkFormat vk::Swapchain::findDepthFormat()
{
    return device.findSupportedFormat({VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT},
                                      VK_IMAGE_TILING_OPTIMAL, VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
}

const bool vk::Swapchain::compatibleWith(Swapchain &other) const
{
    return this->imageFormat == other.getImageFormat() && this->depthFormat == other.getDepthFormat();
}

VkSwapchainKHR vk::Swapchain::getHandle()
{
    return swapchain;
}

VkFramebuffer vk::Swapchain::getFramebuffer(const int index)
{
    return framebuffers[index];
}

VkRenderPass vk::Swapchain::getRenderPass()
{
    return renderPass;
}

VkImageView vk::Swapchain::getImageView(const int index)
{
    return imageViews[index];
}

const size_t vk::Swapchain::getImageCount()
{
    return images.size();
}

VkFormat vk::Swapchain::getImageFormat()
{
    return imageFormat;
}

VkFormat vk::Swapchain::getDepthFormat()
{
    return depthFormat;
}

VkExtent2D vk::Swapchain::getExtent()
{
    return extent;
}

const float vk::Swapchain::getExtentAspectRatio()
{
    return static_cast<float>(extent.width) / static_cast<float>(extent.height);
}

const uint32_t vk::Swapchain::getWidth()
{
    return extent.width;
}

const uint32_t vk::Swapchain::getHeight()
{
    return extent.height;
}

void vk::Swapchain::init(const PresentMode &preferred_present_mode)
{
    createSwapchain(preferred_present_mode);
    createImageViews();
    createRenderPass();
    createDepthResources();
    createFramebuffers();
    createSyncObjects();
}

void vk::Swapchain::createSwapchain(const PresentMode &preferred_present_mode)
{
    Device::SwapchainSupportDetails swapchainSupport = device.getSwapchainSupport();

    VkSurfaceFormatKHR surfaceFormat = chooseSurfaceFormat(swapchainSupport.formats);
    VkPresentModeKHR presentMode = choosePresentMode(swapchainSupport.presentModes, preferred_present_mode);
    VkExtent2D extent = chooseExtent(swapchainSupport.capabilities);

    uint32_t image_count = swapchainSupport.capabilities.minImageCount + 1;
    if (swapchainSupport.capabilities.maxImageCount > 0 && image_count > swapchainSupport.capabilities.maxImageCount)
        image_count = swapchainSupport.capabilities.maxImageCount;

    VkSwapchainCreateInfoKHR swapchain_info = {};
    swapchain_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    swapchain_info.surface = device.getSurface();

    swapchain_info.minImageCount = image_count;
    swapchain_info.imageFormat = surfaceFormat.format;
    swapchain_info.imageColorSpace = surfaceFormat.colorSpace;
    swapchain_info.imageExtent = extent;
    swapchain_info.imageArrayLayers = 1;
    swapchain_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    Device::QueueFamilyIndices indices = device.findPhysicalQueueFamilies();
    uint32_t queueFamilyIndices[] = {*indices.graphicsFamily, *indices.presentFamily};

    if (*indices.graphicsFamily != *indices.presentFamily)
    {
        swapchain_info.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        swapchain_info.queueFamilyIndexCount = 2;
        swapchain_info.pQueueFamilyIndices = queueFamilyIndices;
    }
    else
    {
        swapchain_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        swapchain_info.queueFamilyIndexCount = 0;     // Optional
        swapchain_info.pQueueFamilyIndices = nullptr; // Optional
    }

    swapchain_info.preTransform = swapchainSupport.capabilities.currentTransform;
    swapchain_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;

    swapchain_info.presentMode = presentMode;
    swapchain_info.clipped = VK_TRUE;

    swapchain_info.oldSwapchain = oldSwapchain ? oldSwapchain->getHandle() : VK_NULL_HANDLE;

    if (vkCreateSwapchainKHR(device.getLogicalDevice(), &swapchain_info, nullptr, &swapchain) != VK_SUCCESS)
        throw std::runtime_error("vk::Swapchain::createSwapchain: FAILED TO CREATE SWAPCHAIN");

    // we only specified a minimum number of images in the swap chain, so the implementation is
    // allowed to create a swapchain with more. That's why we'll first query the final number of
    // images with vkGetSwapchainImagesKHR, then resize the container and finally call it again to
    // retrieve the handles.
    vkGetSwapchainImagesKHR(device.getLogicalDevice(), swapchain, &image_count, nullptr);
    images.resize(image_count);
    vkGetSwapchainImagesKHR(device.getLogicalDevice(), swapchain, &image_count, images.data());

    imageFormat = surfaceFormat.format;
    this->extent = extent;
}

void vk::Swapchain::createImageViews()
{
    imageViews.resize(images.size());

    for (size_t i = 0; i < images.size(); i++)
    {
        VkImageViewCreateInfo view_info{};
        view_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        view_info.image = images[i];
        view_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
        view_info.format = imageFormat;
        view_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        view_info.subresourceRange.baseMipLevel = 0;
        view_info.subresourceRange.levelCount = 1;
        view_info.subresourceRange.baseArrayLayer = 0;
        view_info.subresourceRange.layerCount = 1;

        if (vkCreateImageView(device.getLogicalDevice(), &view_info, nullptr, &imageViews[i]) != VK_SUCCESS)
            throw std::runtime_error("vk::Swapchain::createSwapchain: FAILED TO CREATE TEXTURE IMAGE VIEW");
    }
}

void vk::Swapchain::createRenderPass()
{
    VkAttachmentDescription depth_attachment{};
    depth_attachment.format = findDepthFormat();
    depth_attachment.samples = VK_SAMPLE_COUNT_1_BIT;
    depth_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    depth_attachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depth_attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    depth_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depth_attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    depth_attachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    VkAttachmentReference depth_attachment_ref{};
    depth_attachment_ref.attachment = 1;
    depth_attachment_ref.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    VkAttachmentDescription color_attachment = {};
    color_attachment.format = getImageFormat();
    color_attachment.samples = VK_SAMPLE_COUNT_1_BIT;
    color_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    color_attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    color_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    color_attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    color_attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    color_attachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    VkAttachmentReference color_attachment_ref = {};
    color_attachment_ref.attachment = 0;
    color_attachment_ref.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass = {};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &color_attachment_ref;
    subpass.pDepthStencilAttachment = &depth_attachment_ref;

    VkSubpassDependency dependency = {};
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependency.srcAccessMask = 0;
    dependency.srcStageMask =
        VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    dependency.dstSubpass = 0;
    dependency.dstStageMask =
        VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

    std::array<VkAttachmentDescription, 2> attachments = {color_attachment, depth_attachment};
    VkRenderPassCreateInfo render_pass_info = {};
    render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    render_pass_info.attachmentCount = static_cast<uint32_t>(attachments.size());
    render_pass_info.pAttachments = attachments.data();
    render_pass_info.subpassCount = 1;
    render_pass_info.pSubpasses = &subpass;
    render_pass_info.dependencyCount = 1;
    render_pass_info.pDependencies = &dependency;

    if (vkCreateRenderPass(device.getLogicalDevice(), &render_pass_info, nullptr, &renderPass) != VK_SUCCESS)
        throw std::runtime_error("vk::Swapchain::createRenderPass: FAILED TO CREATE RENDER PASS");
}

void vk::Swapchain::createFramebuffers()
{
    framebuffers.resize(getImageCount());

    for (size_t i = 0; i < getImageCount(); i++)
    {
        std::array<VkImageView, 2> attachments = {imageViews[i], depthImageViews[i]};

        VkExtent2D swapchainExtent = getExtent();
        VkFramebufferCreateInfo framebuffer_info = {};
        framebuffer_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebuffer_info.renderPass = renderPass;
        framebuffer_info.attachmentCount = static_cast<uint32_t>(attachments.size());
        framebuffer_info.pAttachments = attachments.data();
        framebuffer_info.width = swapchainExtent.width;
        framebuffer_info.height = swapchainExtent.height;
        framebuffer_info.layers = 1;

        if (vkCreateFramebuffer(device.getLogicalDevice(), &framebuffer_info, nullptr, &framebuffers[i]) != VK_SUCCESS)
            throw std::runtime_error("vk::Swapchain::createFramebuffers: FAILED TO CREATE FRAMEBUFFERS");
    }
}

void vk::Swapchain::createDepthResources()
{
    depthFormat = findDepthFormat();
    VkExtent2D swapchainExtent = getExtent();

    depthImages.resize(getImageCount());
    depthImagesMemory.resize(getImageCount());
    depthImageViews.resize(getImageCount());

    for (int i = 0; i < depthImages.size(); i++)
    {
        VkImageCreateInfo image_info{};
        image_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        image_info.imageType = VK_IMAGE_TYPE_2D;
        image_info.extent.width = swapchainExtent.width;
        image_info.extent.height = swapchainExtent.height;
        image_info.extent.depth = 1;
        image_info.mipLevels = 1;
        image_info.arrayLayers = 1;
        image_info.format = depthFormat;
        image_info.tiling = VK_IMAGE_TILING_OPTIMAL;
        image_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        image_info.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
        image_info.samples = VK_SAMPLE_COUNT_1_BIT;
        image_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        image_info.flags = 0;

        device.createImageWithInfo(image_info, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, depthImages[i],
                                   depthImagesMemory[i]);

        VkImageViewCreateInfo view_info{};
        view_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        view_info.image = depthImages[i];
        view_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
        view_info.format = depthFormat;
        view_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
        view_info.subresourceRange.baseMipLevel = 0;
        view_info.subresourceRange.levelCount = 1;
        view_info.subresourceRange.baseArrayLayer = 0;
        view_info.subresourceRange.layerCount = 1;

        if (vkCreateImageView(device.getLogicalDevice(), &view_info, nullptr, &depthImageViews[i]) != VK_SUCCESS)
        {
            throw std::runtime_error("vk::Swapchain::createDepthResources: FAILED TO CREATE TEXTURE IMAGE VIEW");
        }
    }
}

void vk::Swapchain::createSyncObjects()
{
    imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
    renderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
    inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);
    imagesInFlight.resize(getImageCount(), VK_NULL_HANDLE);

    VkSemaphoreCreateInfo semaphore_info = {};
    semaphore_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fence_info = {};
    fence_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fence_info.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
    {
        if (vkCreateSemaphore(device.getLogicalDevice(), &semaphore_info, nullptr, &imageAvailableSemaphores[i]) !=
                VK_SUCCESS ||
            vkCreateSemaphore(device.getLogicalDevice(), &semaphore_info, nullptr, &renderFinishedSemaphores[i]) !=
                VK_SUCCESS ||
            vkCreateFence(device.getLogicalDevice(), &fence_info, nullptr, &inFlightFences[i]) != VK_SUCCESS)
        {
            throw std::runtime_error("vk::Swapchain::createSyncObjects: FAILED TO CREATE SYNCRONIZATION OBJECTS");
        }
    }
}

VkSurfaceFormatKHR vk::Swapchain::chooseSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &available_formats)
{
    for (const auto &format : available_formats)
    {
        if (format.format == VK_FORMAT_B8G8R8A8_SRGB && format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
            return format;
    }

    return available_formats[0];
}

VkPresentModeKHR vk::Swapchain::choosePresentMode(const std::vector<VkPresentModeKHR> &available_present_modes,
                                                  const PresentMode &preferred_present_mode)
{
    for (const auto &mode : available_present_modes)
    {
        if (mode == static_cast<VkPresentModeKHR>(preferred_present_mode))
        {
#ifndef NDEBUG
            std::cout << "USING PRESENT MODE: " << presentModeName(preferred_present_mode) << std::endl;
#endif
            return mode;
        }
    }

#ifndef NDEBUG
    std::cout << "USING PRESENT MODE: VSync" << std::endl;
#endif

    return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D vk::Swapchain::chooseExtent(const VkSurfaceCapabilitiesKHR &capabilities)
{
    if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
    {
        return capabilities.currentExtent;
    }
    else
    {
        VkExtent2D actual_extent = window.getExtent();
        actual_extent.width = std::max(capabilities.minImageExtent.width,
                                       std::min(capabilities.maxImageExtent.width, actual_extent.width));
        actual_extent.height = std::max(capabilities.minImageExtent.height,
                                        std::min(capabilities.maxImageExtent.height, actual_extent.height));

        return actual_extent;
    }
}

const std::string vk::Swapchain::presentModeName(const PresentMode &present_mode)
{
    switch (present_mode)
    {
    case PresentMode::VSync:
        return "VSync";
    case PresentMode::Immediate:
        return "Immediate";
    case PresentMode::Mailbox:
        return "Mailbox";
    default:
        return "Unknown";
    }
}
