#pragma once

#ifndef GLFW_INCLUDE_VULKAN
#define GLFW_INCLUDE_VULKAN
#endif
#include <GLFW/glfw3.h>

#include "SVKE/Core/Math/Vector.hpp"

#include <cstdint>

// clang-format off

#define COLOR_BLACK  vk::Color(0,   0,   0,   255)
#define COLOR_RED    vk::Color(255, 0,   0,   255)
#define COLOR_GREEN  vk::Color(0,   255, 0,   255)
#define COLOR_BLUE   vk::Color(0,   0,   255, 255)
#define COLOR_YELLOW vk::Color(255, 255, 0,   255)
#define COLOR_ORANGE vk::Color(255, 165, 0,   255)
#define COLOR_CYAN   vk::Color(0,   255, 255, 255)
#define COLOR_PURPLE vk::Color(255, 0,   255, 255)
#define COLOR_WHITE  vk::Color(255, 255, 255, 255)
#define COLOR_GRAY   vk::Color(128, 128, 128, 255)

// clang-format on

namespace vk
{
class Color
{
  public:
    Color();

    Color(const uint8_t r, const uint8_t g, const uint8_t b, const uint8_t a = 255);

    const uint8_t getRedComponent() const;

    const uint8_t getGreenComponent() const;

    const uint8_t getBlueComponent() const;

    const uint8_t getAlphaComponent() const;

    void setRedComponent(const uint8_t r);

    void setGreenComponent(const uint8_t g);

    void setBlueComponent(const uint8_t b);

    void setAlphaComponent(const uint8_t a);

    const Vec3f toVec3() const;

    const Vec4f toVec4() const;

    VkClearColorValue toVkClearColorValue();

  private:
    uint8_t r;
    uint8_t g;
    uint8_t b;
    uint8_t a;
};
} // namespace vk
