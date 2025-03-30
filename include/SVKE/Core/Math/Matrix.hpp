#pragma once

#include "Vector.hpp"
#include <glm/gtc/matrix_transform.hpp>

namespace vk
{
using Mat2f = glm::mat2;
using Mat3f = glm::mat3;
using Mat4f = glm::mat4;

class Matrix
{
  public:
    static const Mat2f identityMat2f();
    static const Mat3f identityMat3f();
    static const Mat4f identityMat4f();

    static const Mat4f rotate(const Mat4f &mat, const float radians, const Vec3f axes);
};
} // namespace vk
