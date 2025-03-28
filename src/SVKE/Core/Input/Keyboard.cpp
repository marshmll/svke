#include "SVKE/Core/Input/Keyboard.hpp"

vk::Keyboard::Keyboard(Window &window) : window(window)
{
    // glfwSetInputMode(window.getHandle(), GLFW_STICKY_KEYS, GLFW_TRUE);
}

const bool vk::Keyboard::isKeyPressed(const Key &key)
{
    return glfwGetKey(window.getHandle(), static_cast<int>(key)) == GLFW_PRESS;
}

const bool vk::Keyboard::wasKeyReleased(const Key &key)
{
    return glfwGetKey(window.getHandle(), static_cast<int>(key)) == GLFW_RELEASE;
}

