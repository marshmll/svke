#include "SVKE/Core/Graphics/TextureSampler.hpp"

vk::TextureSampler::TextureSampler(Device &device, const Config &config) : device(device)
{
    createSampler(config);
}

vk::TextureSampler::~TextureSampler()
{
    vkDestroySampler(device.getLogicalDevice(), sampler, nullptr);
}

const VkSampler &vk::TextureSampler::getSampler() const
{
    return sampler;
}

void vk::TextureSampler::defaultTextureSamplerConfig(Config &config)
{
    config.magnificationFilter = VK_FILTER_LINEAR;
    config.minificationFilter = VK_FILTER_LINEAR;
    config.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
    config.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    config.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    config.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    config.anisotropyEnable = VK_FALSE;
    config.maxAnisotropy = 1.f;
    config.compareEnable = VK_FALSE;
    config.compareOp = VK_COMPARE_OP_ALWAYS;
    config.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
    config.unnormalizedCoordinates = VK_FALSE;
    config.mipLoadBias = 0.0f;
    config.minLod = 0.0f;
    config.maxLod = 0.0f;
}

void vk::TextureSampler::createSampler(const Config &config)
{
    VkSamplerCreateInfo sampler_info{};
    sampler_info.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    sampler_info.magFilter = config.magnificationFilter;
    sampler_info.minFilter = config.minificationFilter;
    sampler_info.mipmapMode = config.mipmapMode;
    sampler_info.addressModeU = config.addressModeU;
    sampler_info.addressModeV = config.addressModeV;
    sampler_info.addressModeW = config.addressModeW;
    sampler_info.anisotropyEnable = config.anisotropyEnable;
    sampler_info.maxAnisotropy = config.maxAnisotropy;
    sampler_info.compareEnable = config.compareEnable;
    sampler_info.compareOp = config.compareOp;
    sampler_info.borderColor = config.borderColor;
    sampler_info.unnormalizedCoordinates = config.unnormalizedCoordinates;
    sampler_info.mipLodBias = config.mipLoadBias;
    sampler_info.minLod = config.minLod;
    sampler_info.maxLod = config.maxLod;

    if (vkCreateSampler(device.getLogicalDevice(), &sampler_info, nullptr, &sampler) != VK_SUCCESS)
        throw std::runtime_error("FAILED TO CREATE TEXTURE SAMPLER");
}
