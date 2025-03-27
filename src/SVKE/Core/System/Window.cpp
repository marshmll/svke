#include "SVKE/Core/System/Window.hpp"

vk::Window::Window(const int width, const int height, const std::string &title)
    : window(nullptr), width(width), height(height), title(title), framebufferResized(false)
{
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
    window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
    glfwSetWindowUserPointer(window, this);
    glfwSetFramebufferSizeCallback(window, vk::Window::framebufferResizedCallback);
}

vk::Window::~Window()
{
    if (window)
        glfwDestroyWindow(window);

    glfwTerminate();
}

const bool vk::Window::shouldClose() const
{
    assert(window != nullptr && "WINDOW HANDLE IS NULL");

    return glfwWindowShouldClose(window);
}

void vk::Window::createSurface(VkInstance &instance, VkSurfaceKHR &surface)
{
    glfwCreateWindowSurface(instance, window, nullptr, &surface);
}

void vk::Window::pollEvents()
{
    assert(window != nullptr && "WINDOW HANDLE IS NULL");

    glfwPollEvents();
}

GLFWwindow *&vk::Window::getHandle()
{
    assert(window != nullptr && "WINDOW HANDLE IS NULL");

    return window;
}

const uint32_t vk::Window::getWidth() const
{
    return static_cast<uint32_t>(width);
}

const uint32_t vk::Window::getHeight() const
{
    return static_cast<uint32_t>(width);
}

VkExtent2D vk::Window::getExtent() const
{
    return {static_cast<uint32_t>(width), static_cast<uint32_t>(height)};
}

const bool vk::Window::wasResized() const
{
    return framebufferResized;
}

void vk::Window::setResized(const bool resized)
{
    framebufferResized = resized;
}

void vk::Window::setWidth(uint32_t width)
{
    this->width = width;
}

void vk::Window::setHeight(uint32_t height)
{
    this->height = height;
}

void vk::Window::framebufferResizedCallback(GLFWwindow *window, int width, int height)
{
    Window *pWindow = reinterpret_cast<Window *>(glfwGetWindowUserPointer(window));

    pWindow->setResized(true);
    pWindow->setWidth(static_cast<uint32_t>(width));
    pWindow->setHeight(static_cast<uint32_t>(height));
}
