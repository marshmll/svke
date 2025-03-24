#pragma once

#include "System/Core/Window.hpp"
#include "System/Core/Device.hpp"
#include "System/Core/Pipeline.hpp"
#include "System/Core/Swapchain.hpp"
#include "Graphics/Rendering/Drawable.hpp"

#ifndef GLM_FORCE_RADIANS
#define GLM_FORCE_RADIANS
#endif
#ifndef GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#endif
#include <glm/glm.hpp>

#include <array>

#define ALIGNAS_SCLR alignas(4)
#define ALIGNAS_VEC2 alignas(8)
#define ALIGNAS_VEC3 alignas(16)
#define ALIGNAS_VEC4 alignas(16)
#define ALIGNAS_MAT2 alignas(16)
#define ALIGNAS_MAT3 alignas(48)
#define ALIGNAS_MAT4 alignas(64)

namespace fl
{
class App
{
    struct PushConstantData
    {
        ALIGNAS_MAT2 glm::mat2 transform{1.f};
        ALIGNAS_VEC2 glm::vec2 offset;
        ALIGNAS_VEC3 glm::vec3 color;
    };

  public:
    App();
    App(const App &) = delete;
    App &operator=(const App &) = delete;

    ~App();

    void run();

  private:
    std::unique_ptr<Window> window;
    std::unique_ptr<Device> device;
    std::unique_ptr<Swapchain> swapchain;
    VkPipelineLayout pipelineLayout;
    std::unique_ptr<Pipeline> pipeline;
    std::vector<VkCommandBuffer> commandBuffers;

    std::unique_ptr<Shader> vertShader;
    std::unique_ptr<Shader> fragShader;

    std::vector<Drawable> drawables;

    void loadDrawables();
    void createPipelineLayout();
    void createPipeline();
    void createCommandBuffers();
    void freeCommandBuffers();
    void drawFrame();
    void drawDrawables(VkCommandBuffer &command_buffer);
    void recreateSwapchain();
    void recordCommandBuffer(uint32_t image_index);
};
} // namespace fl
