#include "SVKE/IO/Mouse.hpp"

vk::Mouse::Mouse(Window &window) : window(window)
{
    setCursorMode(CursorMode::Normal);
    resetCursorData();
}

void vk::Mouse::resetCursorData()
{
    this->data.x = -1.0;
    this->data.y = -1.0;
    this->data.deltaX = -1.f;
    this->data.deltaY = -1.f;
}

void vk::Mouse::setCursorMode(const CursorMode &mode)
{
    glfwSetInputMode(window.getHandle(), GLFW_CURSOR, static_cast<int>(mode));
    this->mode = mode;
    resetCursorData();
}

void vk::Mouse::updateCursorData()
{
    double prev_x = data.x, prev_y = data.y;

    glfwGetCursorPos(window.getHandle(), &data.x, &data.y);

    if (mode == CursorMode::Disabled)
    {
        if (prev_x >= 0.0)
            data.deltaX = data.x - prev_x;

        else
            data.deltaX = 0.f;

        if (prev_y >= 0.0)
            data.deltaY = data.y - prev_y;

        else
            data.deltaY = 0.f;
    }
    else
    {
        data.deltaX = 0.f;
        data.deltaY = 0.f;
    }
}

const vk::Mouse::CursorMode &vk::Mouse::getCursorMode()
{
    return mode;
}

const vk::Mouse::CursorData &vk::Mouse::getCursorData()
{
    return data;
}

const bool vk::Mouse::isRawMotionSupported()
{
    return glfwRawMouseMotionSupported();
}
