#pragma once

#ifndef GLFW_INCLUDE_VULKAN
#define GLFW_INCLUDE_VULKAN
#endif

#include <GLFW/glfw3.h>

#include <cassert>
#include <string>
#include <iostream>

namespace vk
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

    const bool wasResized() const;

    const bool isFullscreen() const;

    void setResized(const bool resized);

    void setPosition(const int x, const int y);

    void setWidth(const uint32_t width);

    void setHeight(const uint32_t height);

    void setFullscreen(const bool fullscreen);

  private:
    GLFWwindow *window;
    GLFWmonitor *monitor;
    int width;
    int height;
    int x;
    int y;
    std::string title;
    bool framebufferResized;
    bool fullscreen;

    static void framebufferResizedCallback(GLFWwindow *window, int width, int height);
};
} // namespace vk
