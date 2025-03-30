#pragma once

#ifndef GLM_FORCE_RADIANS
#define GLM_FORCE_RADIANS
#endif
#ifndef GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#endif
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

namespace vk
{
class Angle
{
  public:
    static constexpr float Rad45 = glm::pi<float>() / 4.f;
    static constexpr float Rad90 = glm::pi<float>() / 2.f;
    static constexpr float Rad180 = glm::pi<float>();
    static constexpr float Rad360 = glm::two_pi<float>();

    static const float toRadians(const float degrees);
    static const float toDegrees(const float radians);
};
} // namespace vk
