#include "SVKE/Core/Graphics/Texture.hpp"

vk::Texture::Texture() : width(0), height(0), pixels(nullptr), size(0)
{
}

vk::Texture::~Texture()
{
    stbi_image_free(pixels);
}

const bool vk::Texture::loadFromFile(const std::string &path)
{
    pixels = stbi_load(path.c_str(), &width, &height, &channels, STBI_rgb_alpha);
    size = width * height * 4;

    if (!pixels)
    {
        std::cerr << "vk::Texture::loadFromFile: FAILED TO LOAD IMAGE FROM FILE " << path << std::endl;
        return false;
    }

    return true;
}

const int vk::Texture::getWidth() const
{
    return width;
}

const int vk::Texture::getHeight() const
{
    return height;
}

const int vk::Texture::getChannels() const
{
    return channels;
}

const vk::Texture::Size vk::Texture::getSize() const
{
    return size;
}

vk::Texture::Pixels &vk::Texture::getPixels()
{
    return pixels;
}
