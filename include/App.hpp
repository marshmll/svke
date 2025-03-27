#pragma once

#include "SVKE/Rendering/Camera.hpp"
#include "SVKE/Rendering/Resources/Drawable.hpp"
#include "SVKE/Core/System/Window.hpp"
#include "SVKE/Core/System/Device.hpp"
#include "SVKE/Rendering/Systems/Renderer.hpp"
#include "SVKE/Rendering/Systems/RenderSystem.hpp"

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
