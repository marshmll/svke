#include "System/Core/Window.hpp"

fl::Window::Window(const int width, const int height, const std::string &title)
    : window(nullptr), width(width), height(height), title(title)
{
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
}

fl::Window::~Window()
{
    if (window)
        glfwDestroyWindow(window);

    glfwTerminate();
}

const bool fl::Window::shouldClose() const
{
    assert(window != nullptr && "WINDOW HANDLE IS NULL");

    return glfwWindowShouldClose(window);
}

void fl::Window::createSurface(VkInstance &instance, VkSurfaceKHR &surface)
{
    glfwCreateWindowSurface(instance, window, nullptr, &surface);
}

void fl::Window::pollEvents()
{
    assert(window != nullptr && "WINDOW HANDLE IS NULL");

    glfwPollEvents();
}

GLFWwindow *&fl::Window::getHandle()
{
    assert(window != nullptr && "WINDOW HANDLE IS NULL");

    return window;
}

const uint32_t fl::Window::getWidth() const
{
    return static_cast<uint32_t>(width);
}

const uint32_t fl::Window::getHeight() const
{
    return static_cast<uint32_t>(width);
}

VkExtent2D fl::Window::getExtent() const
{
    return {static_cast<uint32_t>(width), static_cast<uint32_t>(height)};
}
