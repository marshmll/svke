#pragma once

#ifndef GLM_FORCE_RADIANS
#define GLM_FORCE_RADIANS
#endif
#ifndef GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#endif
#include <glm/glm.hpp>

#ifndef GLFW_INCLUDE_VULKAN
#define GLFW_INCLUDE_VULKAN
#endif
#include <GLFW/glfw3.h>

#include <cstdint>

// clang-format off

#define COLOR_BLACK  fl::Color(0,   0,   0,   255)
#define COLOR_RED    fl::Color(255, 0,   0,   255)
#define COLOR_GREEN  fl::Color(0,   255, 0,   255)
#define COLOR_BLUE   fl::Color(0,   0,   255, 255)
#define COLOR_YELLOW fl::Color(255, 255, 0,   255)
#define COLOR_CYAN   fl::Color(0,   255, 255, 255)
#define COLOR_PURPLE fl::Color(255, 0,   255, 255)
#define COLOR_WHITE  fl::Color(255, 255, 255, 255)
#define COLOR_GRAY   fl::Color(128, 128, 128, 255)

// clang-format on

namespace fl
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

    const glm::vec3 toVec3() const;

    const glm::vec4 toVec4() const;

    VkClearColorValue toVkClearColorValue();

  private:
    uint8_t r;
    uint8_t g;
    uint8_t b;
    uint8_t a;
};
} // namespace fl
