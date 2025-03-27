#include "SVKE/Core/Graphics/Color.hpp"

fl::Color::Color() : r(0), g(0), b(0), a(255)
{
}

fl::Color::Color(const uint8_t r, const uint8_t g, const uint8_t b, const uint8_t a) : r(r), g(g), b(b), a(a)
{
}

const uint8_t fl::Color::getRedComponent() const
{
    return r;
}

const uint8_t fl::Color::getGreenComponent() const
{
    return g;
}

const uint8_t fl::Color::getBlueComponent() const
{
    return b;
}

const uint8_t fl::Color::getAlphaComponent() const
{
    return a;
}

void fl::Color::setRedComponent(const uint8_t r)
{
    this->r = r;
}

void fl::Color::setGreenComponent(const uint8_t g)
{
    this->g = g;
}

void fl::Color::setBlueComponent(const uint8_t b)
{
    this->b = b;
}

void fl::Color::setAlphaComponent(const uint8_t a)
{
    this->a = a;
}

const glm::vec3 fl::Color::toVec3() const
{
    return {static_cast<float>(r) / 255.f, static_cast<float>(g) / 255.f, static_cast<float>(b) / 255.f};
}

const glm::vec4 fl::Color::toVec4() const
{
    return {static_cast<float>(r) / 255.f, static_cast<float>(g) / 255.f, static_cast<float>(b) / 255.f,
            static_cast<float>(a) / 255.f};
}

VkClearColorValue fl::Color::toVkClearColorValue()
{
    glm::vec4 vec4 = toVec4();

    return {vec4.x, vec4.y, vec4.z, vec4.w};
}
