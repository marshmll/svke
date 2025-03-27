#include "Graphics/Projection/Camera.hpp"

fl::Camera::Camera() : projectionMatrix(1.f)
{
}

void fl::Camera::setOrthograpicProjection(const float left, const float right, const float top, const float bottom,
                                          const float near, const float far)
{
    projectionMatrix = glm::mat4{1.f};

    projectionMatrix[0][0] = 2.f / (right - left);
    projectionMatrix[1][1] = 2.f / (bottom - top);
    projectionMatrix[2][2] = 1.f / (far - near);
    projectionMatrix[3][0] = -(right + left) / (right - left);
    projectionMatrix[3][1] = -(bottom + top) / (bottom - top);
    projectionMatrix[3][2] = -near / (far - near);
}

void fl::Camera::setPerspectiveProjection(const float fovy, const float aspect_ratio, const float near, const float far)
{
    assert(glm::abs(aspect_ratio - std::numeric_limits<float>::epsilon() > 0.f));

    const float tan_half_fovy = tan(fovy / 2.f);
    projectionMatrix = glm::mat4{1.f};

    projectionMatrix[0][0] = 1.f / (aspect_ratio * tan_half_fovy);
    projectionMatrix[1][1] = 1.f / (tan_half_fovy);
    projectionMatrix[2][2] = far / (far - near);
    projectionMatrix[2][3] = 1.f;
    projectionMatrix[3][2] = -(far * near) / (far - near);
}

const glm::mat4 &fl::Camera::getProjectionMatrix() const
{
    return projectionMatrix;
}
