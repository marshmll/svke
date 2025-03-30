#include "SVKE/Core/Math/Angle.hpp"

const float vk::Angle::toRadians(const float degrees)
{
    return glm::radians(degrees);
}

const float vk::Angle::toDegrees(const float radians)
{
    return glm::degrees(radians);
}
