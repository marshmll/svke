#pragma once

#include "System/Core/Window.hpp"
#include "System/Core/Device.hpp"

#include <string>
#include <vector>

namespace fl
{
class Swapchain
{
  public:
    static constexpr int MAX_FRAMES_IN_FLIGHT = 2;

    Swapchain(Device &device, Window &window);
    ~Swapchain();

    Swapchain(const Swapchain &) = delete;
    Swapchain &operator=(const Swapchain &) = delete;

    VkFramebuffer getFramebuffer(const int index);

    VkRenderPass getRenderPass();

    VkImageView getImageView(const int index);

    const size_t getImageCount();

    VkFormat getImageFormat();

    VkExtent2D getExtent();

    const uint32_t getWidth();

    const uint32_t getHeight();

    const float extentAspectRatio();

    VkFormat findDepthFormat();

    VkResult acquireNextImage(uint32_t &image_index);

    VkResult submitCommandBuffers(const VkCommandBuffer &buffers, uint32_t &image_index);

  private:
    Device &device;
    Window &window;

    VkFormat imageFormat;
    VkExtent2D extent;

    std::vector<VkFramebuffer> framebuffers;
    VkRenderPass renderPass;

    std::vector<VkImage> depthImages;
    std::vector<VmaAllocation> depthImagesMemory;
    std::vector<VkImageView> depthImageViews;
    std::vector<VkImage> images;
    std::vector<VkImageView> imageViews;

    VkSwapchainKHR swapchain;

    std::vector<VkSemaphore> imageAvailableSemaphores;
    std::vector<VkSemaphore> renderFinishedSemaphores;
    std::vector<VkFence> inFlightFences;
    std::vector<VkFence> imagesInFlight;
    size_t currentFrame = 0;

    void createSwapchain();

    void createImageViews();

    void createDepthResources();

    void createRenderPass();

    void createFramebuffers();

    void createSyncObjects();

    VkSurfaceFormatKHR chooseSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &available_formats);

    VkPresentModeKHR choosePresentMode(const std::vector<VkPresentModeKHR> &available_present_modes);

    VkExtent2D chooseExtent(const VkSurfaceCapabilitiesKHR &capabilities);
};
} // namespace fl
