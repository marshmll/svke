#pragma once

#include "SVKE/Core/System/Window.hpp"
#include "SVKE/Core/System/Device.hpp"

#include <array>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <limits>
#include <set>
#include <stdexcept>
#include <memory>

namespace vk
{
class Swapchain
{
  public:
    static constexpr int MAX_FRAMES_IN_FLIGHT = 2;

    enum class PresentMode : int
    {
        Immediate = VK_PRESENT_MODE_IMMEDIATE_KHR,
        Mailbox = VK_PRESENT_MODE_MAILBOX_KHR,
        SharedContinuousRefresh = VK_PRESENT_MODE_SHARED_CONTINUOUS_REFRESH_KHR,
        SharedDemandRefresh = VK_PRESENT_MODE_SHARED_DEMAND_REFRESH_KHR,
        VSync = VK_PRESENT_MODE_FIFO_KHR,
        VSyncLatest = VK_PRESENT_MODE_FIFO_LATEST_READY_EXT,
        VSyncRelaxed = VK_PRESENT_MODE_FIFO_RELAXED_KHR
    };

    Swapchain(Device &device, Window &window, const PresentMode &preferred_present_mode = PresentMode::Mailbox);
    Swapchain(Device &device, Window &window, std::shared_ptr<Swapchain> &previous,
              const PresentMode &preferred_present_mode = PresentMode::Mailbox);
    ~Swapchain();

    Swapchain(const Swapchain &) = delete;
    Swapchain &operator=(const Swapchain &) = delete;

    VkResult submitCommandBuffers(const VkCommandBuffer &buffers, uint32_t &image_index);

    VkResult acquireNextImage(uint32_t &image_index);

    VkFormat findDepthFormat();

    const bool compatibleWith(Swapchain &other) const;

    VkSwapchainKHR getHandle();

    VkFramebuffer getFramebuffer(const int index);

    VkRenderPass getRenderPass();

    VkImageView getImageView(const int index);

    const size_t getImageCount();

    VkFormat getImageFormat();

    VkFormat getDepthFormat();

    VkExtent2D getExtent();

    const uint32_t getWidth();

    const uint32_t getHeight();

    const float getExtentAspectRatio();

  private:
    Device &device;
    Window &window;

    VkFormat imageFormat;
    VkFormat depthFormat;
    VkExtent2D extent;

    std::vector<VkFramebuffer> framebuffers;
    VkRenderPass renderPass;

    std::vector<VkImage> depthImages;
    std::vector<VmaAllocation> depthImagesMemory;
    std::vector<VkImageView> depthImageViews;
    std::vector<VkImage> images;
    std::vector<VkImageView> imageViews;

    VkSwapchainKHR swapchain;
    std::shared_ptr<Swapchain> oldSwapchain;

    std::vector<VkSemaphore> imageAvailableSemaphores;
    std::vector<VkSemaphore> renderFinishedSemaphores;
    std::vector<VkFence> inFlightFences;
    std::vector<VkFence> imagesInFlight;
    size_t currentFrame;

    void init(const PresentMode &preferred_present_mode);

    void createSwapchain(const PresentMode &preferred_present_mode);

    void createImageViews();

    void createDepthResources();

    void createRenderPass();

    void createFramebuffers();

    void createSyncObjects();

    VkSurfaceFormatKHR chooseSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &available_formats);

    VkPresentModeKHR choosePresentMode(const std::vector<VkPresentModeKHR> &available_present_modes,
                                       const PresentMode &preferred_present_mode);

    VkExtent2D chooseExtent(const VkSurfaceCapabilitiesKHR &capabilities);

    const std::string presentModeName(const PresentMode &present_mode);
};
} // namespace vk
