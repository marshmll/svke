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

namespace fl
{
class Camera
{
  public:
    Camera();

    void setOrthograpicProjection(const float left, const float right, const float top, const float bottom,
                                  const float near, const float far);

    void setPerspectiveProjection(const float fovy, const float aspect_ratio, const float near, const float far);

    const glm::mat4 &getProjectionMatrix() const;

  private:
    glm::mat4 projectionMatrix;
};
} // namespace fl
