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
    TextureImage(Device &device, Texture &texture);

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
