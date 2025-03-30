#include "SVKE/Core/Math/Matrix.hpp"

const vk::Mat4f vk::Matrix::rotate(const Mat4f &mat, const float radians, const Vec3f axes)
{
    return glm::rotate(mat, radians, axes);
}

const vk::Mat2f vk::Matrix::identityMat2f()
{
    return Mat2f{1.f};
}

const vk::Mat3f vk::Matrix::identityMat3f()
{
    return Mat3f{1.f};
}

const vk::Mat4f vk::Matrix::identityMat4f()
{
    return Mat4f{1.f};
}
