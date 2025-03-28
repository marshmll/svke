#pragma once

#include "SVKE/Core/System/Window.hpp"

namespace vk
{
class Mouse
{
  public:
    enum class CursorMode : int
    {
        Normal = GLFW_CURSOR_NORMAL,
        Hidden = GLFW_CURSOR_HIDDEN,
        Disabled = GLFW_CURSOR_DISABLED
    };

    struct CursorData
    {
        double x = 0.0;
        double y = 0.0;
        float deltaX = 0.f;
        float deltaY = 0.f;
    };

    Mouse(Window &window);
    Mouse(const Mouse &) = delete;
    Mouse &operator=(const Mouse &) = delete;
    Mouse(Mouse &&) = delete;
    Mouse &operator=(Mouse &&) = delete;

    void resetCursorData();

    void setCursorMode(const CursorMode &mode);

    void updateCursorData();

    const CursorMode &getCursorMode();

    const CursorData &getCursorData();

    static const bool isRawMotionSupported();

  private:
    Window &window;

    CursorMode mode;
    CursorData data;
};
} // namespace vk
