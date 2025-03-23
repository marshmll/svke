#pragma once

#include "System/Core/Window.hpp"
#include "System/Core/Device.hpp"
#include "System/Core/Pipeline.hpp"
#include "System/Core/Swapchain.hpp"
#include "Graphics/Core/Model.hpp"

#include <array>

namespace fl
{
class App
{
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

    std::unique_ptr<Model> model;

    void loadModels();
    void createPipelineLayout();
    void createPipeline();
    void createCommandBuffers();
    void drawFrame();
};
} // namespace fl
