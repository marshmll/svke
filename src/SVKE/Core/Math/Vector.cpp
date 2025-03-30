#include "SVKE/Core/Math/Vector.hpp"

const float vk::Vector::dot(Vec2f first, Vec2f second)
{
    return glm::dot(first, second);
}

const float vk::Vector::dot(Vec3f first, Vec3f second)
{
    return glm::dot(first, second);
}

const float vk::Vector::dot(Vec4f first, Vec4f second)
{
    return glm::dot(first, second);
}

const vk::Vec2f vk::Vector::normalize(Vec2f vec)
{
    return glm::normalize(vec);
}

const vk::Vec3f vk::Vector::normalize(Vec3f vec)
{
    return glm::normalize(vec);
}

const vk::Vec4f vk::Vector::normalize(Vec4f vec)
{
    return glm::normalize(vec);
}
