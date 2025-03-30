#pragma once

#include "SVKE/Core.hpp"
#include "SVKE/Rendering.hpp"
#include "SVKE/Utils.hpp"

namespace vk
{
class App
{
  public:
    App();
    App(const App &) = delete;
    App &operator=(const App &) = delete;
    App(App &&) = delete;
    App &operator=(App &&) = delete;

    void run();

  private:
    std::unique_ptr<Window> window;
    std::unique_ptr<Device> device;
    std::unique_ptr<Renderer> renderer;
    std::unique_ptr<DescriptorPool> globalPool;
    Object::Map objects;

    void createWindow();

    void createDevice();

    void createRenderer();

    void createGlobalPool();

    void loadObjects();
};
} // namespace vk
