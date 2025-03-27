#include "SVKE/Core/Graphics/Color.hpp"

vk::Color::Color() : r(0), g(0), b(0), a(255)
{
}

vk::Color::Color(const uint8_t r, const uint8_t g, const uint8_t b, const uint8_t a) : r(r), g(g), b(b), a(a)
{
}

const uint8_t vk::Color::getRedComponent() const
{
    return r;
}

const uint8_t vk::Color::getGreenComponent() const
{
    return g;
}

const uint8_t vk::Color::getBlueComponent() const
{
    return b;
}

const uint8_t vk::Color::getAlphaComponent() const
{
    return a;
}

void vk::Color::setRedComponent(const uint8_t r)
{
    this->r = r;
}

void vk::Color::setGreenComponent(const uint8_t g)
{
    this->g = g;
}

void vk::Color::setBlueComponent(const uint8_t b)
{
    this->b = b;
}

void vk::Color::setAlphaComponent(const uint8_t a)
{
    this->a = a;
}

const glm::vec3 vk::Color::toVec3() const
{
    return {static_cast<float>(r) / 255.f, static_cast<float>(g) / 255.f, static_cast<float>(b) / 255.f};
}

const glm::vec4 vk::Color::toVec4() const
{
    return {static_cast<float>(r) / 255.f, static_cast<float>(g) / 255.f, static_cast<float>(b) / 255.f,
            static_cast<float>(a) / 255.f};
}

VkClearColorValue vk::Color::toVkClearColorValue()
{
    glm::vec4 vec4 = toVec4();

    return {vec4.x, vec4.y, vec4.z, vec4.w};
}
