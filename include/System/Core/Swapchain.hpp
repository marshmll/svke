#pragma once

#include "System/Core/Window.hpp"
#include "System/Core/Device.hpp"

#include <array>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <limits>
#include <set>
#include <stdexcept>
#include <memory>

namespace fl
{
class Swapchain
{
  public:
    static constexpr int MAX_FRAMES_IN_FLIGHT = 2;

    Swapchain(Device &device, Window &window);
    Swapchain(Device &device, Window &window, std::shared_ptr<Swapchain> &previous);
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

    const float extentAspectRatio();

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

    void init();

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
