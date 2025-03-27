#pragma once

#include "Graphics/Projection/Camera.hpp"
#include "Graphics/Rendering/Drawable.hpp"
#include "System/Core/Window.hpp"
#include "System/Core/Device.hpp"
#include "System/Rendering/Renderer.hpp"
#include "System/Rendering/Systems/RenderSystem.hpp"

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
    std::unique_ptr<Renderer> renderer;
    std::vector<Drawable> drawables;

    void createWindow();

    void createDevice();

    void createRenderer();

    void loadDrawables();
};
} // namespace fl
