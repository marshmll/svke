#pragma once

#include "SVKE/Core/System/Device.hpp"
#include "SVKE/Core/System/Memory/Buffer.hpp"
#include "SVKE/Core/Graphics/Texture.hpp"
#include "SVKE/Core/Graphics/TextureSampler.hpp"

namespace vk
{
class TextureImage
{
  public:
    enum class Format : int
    {
        RGBA = VK_FORMAT_R8G8B8A8_SRGB
    };

    enum class Tiling : int
    {
        Linear = VK_IMAGE_TILING_LINEAR,
        Optimal = VK_IMAGE_TILING_OPTIMAL
    };

    TextureImage(Device &device, Texture &texture, const Format &format, const Tiling &tiling,
                 const VkImageUsageFlags usage);

    TextureImage(const TextureImage &) = delete;
    TextureImage &operator=(const TextureImage &) = delete;

    ~TextureImage();

    const VkDescriptorImageInfo getDescriptorInfo(TextureSampler &sampler) const;

  private:
    Device &device;

    VkImage image;
    VmaAllocation allocation;
    VkFormat format;
    VkImageView imageView;

    void createImage(Texture &texture, const VkImageTiling tiling, const VkImageUsageFlags usage);

    void transitionImageLayout(const VkImageLayout old_layout, const VkImageLayout new_layout);

    void copyTextureToImage(Texture &texture);

    void createImageView();
};
} // namespace vk
