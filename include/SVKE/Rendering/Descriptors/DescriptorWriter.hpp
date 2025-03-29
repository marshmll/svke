#pragma once

#include "SVKE/Core/System/Device.hpp"
#include "SVKE/Rendering/Descriptors/DescriptorPool.hpp"
#include "SVKE/Rendering/Descriptors/DescriptorSetLayout.hpp"
#include "SVKE/Rendering/Descriptors/DescriptorSet.hpp"

#include <memory>
#include <unordered_map>
#include <vector>

namespace vk
{
class DescriptorWriter
{
  public:
    DescriptorWriter(DescriptorSetLayout &set_layout, DescriptorPool &pool);

    DescriptorWriter &writeBuffer(const uint32_t binding, VkDescriptorBufferInfo &buffer_info);
    DescriptorWriter &writeImage(const uint32_t binding, VkDescriptorImageInfo &image_info);

    const bool build(DescriptorSet &set);
    void overwrite(DescriptorSet &set);

  private:
    DescriptorSetLayout &setLayout;
    DescriptorPool &pool;
    std::vector<VkWriteDescriptorSet> writes;
};
} // namespace vk
