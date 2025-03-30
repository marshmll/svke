#pragma once

#include "SVKE/Core/Math/Vector.hpp"
#include "SVKE/Core/Math/Matrix.hpp"

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

    void setViewDirection(const Vec3f &position, const Vec3f &direction, const Vec3f &up = {0.f, -1.f, 0.f});

    void setViewTarget(const Vec3f &position, const Vec3f &target, const Vec3f &up = {0.f, -1.f, 0.f});

    void setViewYXZ(const Vec3f &position, const Vec3f &rotation);

    const Mat4f &getProjectionMatrix() const;

    const Mat4f &getViewMatrix() const;

  private:
    Mat4f projectionMatrix;
    Mat4f viewMatrix;
};
} // namespace vk
