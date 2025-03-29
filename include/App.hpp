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
        ALIGNAS_MAT4 glm::mat4 projectionViewMatrix{1.f};
        ALIGNAS_VEC3 glm::vec3 lightDirection = glm::normalize(glm::vec3{1.f, -3.f, -1.f});
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
    std::vector<Object> objects;

    void createWindow();

    void createDevice();

    void createRenderer();

    void createGlobalPool();

    void loadObjects();
};
} // namespace vk
