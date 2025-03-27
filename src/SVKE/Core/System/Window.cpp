#include "SVKE/Core/System/Window.hpp"

vk::Window::Window(const int width, const int height, const std::string &title)
    : window(nullptr), width(width), height(height), title(title), framebufferResized(false), fullscreen(false)
{
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

    window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
    monitor = glfwGetPrimaryMonitor();

    glfwSetWindowUserPointer(window, this);
    glfwSetFramebufferSizeCallback(window, vk::Window::framebufferResizedCallback);
    glfwGetWindowPos(window, &x, &y);
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
    assert(window != nullptr && "WINDOW HANDLE IS NULL");

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
    assert(window != nullptr && "WINDOW HANDLE IS NULL");

    return static_cast<uint32_t>(width);
}

const uint32_t vk::Window::getHeight() const
{
    assert(window != nullptr && "WINDOW HANDLE IS NULL");

    return static_cast<uint32_t>(width);
}

VkExtent2D vk::Window::getExtent() const
{
    assert(window != nullptr && "WINDOW HANDLE IS NULL");

    return {static_cast<uint32_t>(width), static_cast<uint32_t>(height)};
}

const bool vk::Window::wasResized() const
{
    assert(window != nullptr && "WINDOW HANDLE IS NULL");

    return framebufferResized;
}

const bool vk::Window::isFullscreen() const
{
    assert(window != nullptr && "WINDOW HANDLE IS NULL");

    return fullscreen;
}

void vk::Window::setResized(const bool resized)
{
    assert(window != nullptr && "WINDOW HANDLE IS NULL");

    framebufferResized = resized;
}

void vk::Window::setPosition(const int x, const int y)
{
    assert(window != nullptr && "WINDOW HANDLE IS NULL");

    this->x = x;
    this->y = y;

    glfwSetWindowPos(window, x, y);
}

void vk::Window::setWidth(const uint32_t width)
{
    assert(window != nullptr && "WINDOW HANDLE IS NULL");

    this->width = width;
}

void vk::Window::setHeight(const uint32_t height)
{
    assert(window != nullptr && "WINDOW HANDLE IS NULL");

    this->height = height;
}

void vk::Window::setFullscreen(const bool fullscreen)
{
    assert(window != nullptr && "WINDOW HANDLE IS NULL");

    if (this->fullscreen == fullscreen)
        return;

    this->fullscreen = fullscreen;

    if (fullscreen)
    {
        // backup window position and window size
        glfwGetWindowPos(window, &x, &y);

        std::cout << x << " " << y << " " << width << " " << height << std::endl;

        // get resolution of monitor
        const GLFWvidmode *mode = glfwGetVideoMode(monitor);

        // switch to full screen
        glfwSetWindowMonitor(window, monitor, 0, 0, mode->width, mode->height, 0);
    }
    else
    {
        // restore last window size and position
        glfwSetWindowMonitor(window, nullptr, x, y, width, height, 0);
    }
}

void vk::Window::framebufferResizedCallback(GLFWwindow *window, int width, int height)
{
    Window *pWindow = reinterpret_cast<Window *>(glfwGetWindowUserPointer(window));

    pWindow->setResized(true);
    pWindow->setWidth(static_cast<uint32_t>(width));
    pWindow->setHeight(static_cast<uint32_t>(height));
}
