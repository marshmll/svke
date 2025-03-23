#pragma once

#ifndef GLFW_INCLUDE_VULKAN
#define GLFW_INCLUDE_VULKAN
#endif

#include <GLFW/glfw3.h>

#include <cassert>
#include <string>

namespace fl
{
class Window
{
  public:
    Window() = default;
    Window(const int width, const int height, const std::string &title = "Untitled Window");
    Window(const Window &) = delete;
    Window &operator=(const Window &) = delete;
    ~Window();

    const bool shouldClose() const;

    void createSurface(VkInstance &instance, VkSurfaceKHR &surface);

    void pollEvents();

    GLFWwindow *&getHandle();

    const uint32_t getWidth() const;

    const uint32_t getHeight() const;

    VkExtent2D getExtent() const;

  private:
    GLFWwindow *window;
    int width;
    int height;
    std::string title;
};
} // namespace fl
