#pragma once

#include "SVKE/Core/System/Device.hpp"

namespace vk
{
class TextureSampler
{
  public:
    struct Config
    {
        VkFilter magnifyingFilter;
        VkFilter minifyingFilter;
        VkSamplerMipmapMode mipmapMode;
        VkSamplerAddressMode addressModeU;
        VkSamplerAddressMode addressModeV;
        VkSamplerAddressMode addressModeW;
        VkBool32 anisotropyEnable;
        float maxAnisotropy;
        VkBool32 compareEnable;
        VkCompareOp compareOp;
        VkBorderColor borderColor;
        VkBool32 unnormalizedCoordinates;
        float mipLoadBias;
        float minLod;
        float maxLod;
    };

    TextureSampler(Device &device, const Config &config);
    TextureSampler(const TextureSampler &) = delete;
    TextureSampler &operator=(const TextureSampler &) = delete;

    ~TextureSampler();

    VkSampler &getSampler();

    static void defaultTextureSamplerConfig(Config &config);

  private:
    Device &device;

    VkSampler sampler;

    void createSampler(const Config &config);
};
} // namespace vk
