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
using Vec2f = glm::vec2;
using Vec3f = glm::vec3;
using Vec4f = glm::vec4;

class Vector
{
  public:
    static const float dot(Vec2f first, Vec2f second);
    static const float dot(Vec3f first, Vec3f second);
    static const float dot(Vec4f first, Vec4f second);

    static const Vec2f normalize(Vec2f vec);
    static const Vec3f normalize(Vec3f vec);
    static const Vec4f normalize(Vec4f vec);
};
} // namespace vk
