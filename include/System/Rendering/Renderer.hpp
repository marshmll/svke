#pragma once

#include "System/Core/Window.hpp"
#include "System/Core/Device.hpp"
#include "System/Core/Swapchain.hpp"
#include "Graphics/Color/Color.hpp"

#ifndef GLM_FORCE_RADIANS
#define GLM_FORCE_RADIANS
#endif
#ifndef GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#endif
#include <glm/glm.hpp>

#include <array>

namespace fl
{
class Renderer
{
  public:
    Renderer(Device &device, Window &window, const Color &clear_color = COLOR_BLACK);
    Renderer(const Renderer &) = delete;
    Renderer &operator=(const Renderer &) = delete;

    ~Renderer();

    VkCommandBuffer beginFrame();

    void endFrame();

    void beginRenderPass(VkCommandBuffer &command_buffer);

    void endRenderPass(VkCommandBuffer &command_buffer);

    const bool isFrameInProgress() const;

    const int getCurrentFrameIndex() const;

    VkCommandBuffer &getCurrentCommandBuffer();

    VkRenderPass getRenderPass();

    const float getAspectRatio() const;

  private:
    Device &device;
    Window &window;

    Color clearColor;

    std::unique_ptr<Swapchain> swapchain;
    std::vector<VkCommandBuffer> commandBuffers;

    uint32_t currentImageIndex;
    int currentFrameIndex;
    bool frameInProgress;

    void createCommandBuffers();

    void freeCommandBuffers();

    void recreateSwapchain();
};
} // namespace fl
