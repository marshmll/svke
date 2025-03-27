#pragma once

#ifndef GLM_FORCE_RADIANS
#define GLM_FORCE_RADIANS
#endif
#ifndef GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#endif
#include <glm/glm.hpp>

#include <cassert>
#include <limits>

namespace vk
{
class Camera
{
  public:
    Camera();

    void setOrthograpicProjection(const float left, const float right, const float top, const float bottom,
                                  const float near, const float far);

    void setPerspectiveProjection(const float fovy, const float aspect_ratio, const float near, const float far);

    void setViewDirection(const glm::vec3 &position, const glm::vec3 &direction,
                          const glm::vec3 &up = {0.f, -1.f, 0.f});

    void setViewTarget(const glm::vec3 &position, const glm::vec3 &target, const glm::vec3 &up = {0.f, -1.f, 0.f});

    void setViewYXZ(const glm::vec3 &position, const glm::vec3 &rotation);

    const glm::mat4 &getProjectionMatrix() const;

    const glm::mat4 &getViewMatrix() const;

  private:
    glm::mat4 projectionMatrix;
    glm::mat4 viewMatrix;
};
} // namespace vk
