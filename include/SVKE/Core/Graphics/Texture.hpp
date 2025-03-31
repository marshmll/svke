#pragma once

#ifndef GLFW_INCLUDE_VULKAN
#define GLFW_INCLUDE_VULKAN
#endif

#include <GLFW/glfw3.h>
#include <stb_image.h>

#include <string>
#include <iostream>

namespace vk
{
class Texture
{
  public:
    using Pixels = stbi_uc *;
    using Size = VkDeviceSize;

    Texture();
    Texture(const Texture &) = delete;
    Texture &operator=(const Texture &) = delete;
    Texture(Texture &&) = default;
    Texture &operator=(Texture &&) = default;

    ~Texture();

    [[nodiscard]]
    const bool loadFromFile(const std::string &path);

    [[nodiscard]]
    const int getWidth() const;

    [[nodiscard]]
    const int getHeight() const;

    [[nodiscard]]
    const int getChannels() const;

    [[nodiscard]]
    Pixels &getPixels();

    [[nodiscard]]
    const Size getSize() const;

  private:
    int width;
    int height;
    int channels;
    Pixels pixels;
    Size size;
};
} // namespace vk
