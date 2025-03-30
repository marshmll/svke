#pragma once

#include "SVKE/Core.hpp"
#include "SVKE/Rendering.hpp"
#include "SVKE/Utils.hpp"

namespace vk
{
class App
{
  public:
    struct GlobalUBO
    {
        ALIGNAS_MAT4 glm::mat4 projectionMatrix{1.f};
        ALIGNAS_MAT4 glm::mat4 viewMatrix{1.f};
        ALIGNAS_VEC4 glm::vec4 ambientLightColor{1.f, 1.f, 1.f, .2f};
        ALIGNAS_VEC3 glm::vec3 lightPosition{-1.f};
        ALIGNAS_VEC4 glm::vec4 lightColor{1.f}; // w component is light intensity
    };

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
