#pragma once

#include "SVKE/Core/System/Device.hpp"
#include "SVKE/Rendering/Descriptors/DescriptorSet.hpp"

#include <memory>
#include <vector>

namespace vk
{
class DescriptorPool
{
  public:
    class Builder
    {
      public:
        Builder(Device &device);

        Builder &addPoolSize(VkDescriptorType descriptor_type, const uint32_t count);
        Builder &setPoolFlags(VkDescriptorPoolCreateFlags flags);
        Builder &setMaxSets(const uint32_t count);

        std::unique_ptr<DescriptorPool> build() const;

      private:
        Device &device;
        std::vector<VkDescriptorPoolSize> poolSizes;
        uint32_t maxSets;
        VkDescriptorPoolCreateFlags poolFlags;
    };

    DescriptorPool(Device &device, const uint32_t max_sets, VkDescriptorPoolCreateFlags pool_flags,
                   const std::vector<VkDescriptorPoolSize> &pool_sizes);

    DescriptorPool(const DescriptorPool &) = delete;
    DescriptorPool &operator=(const DescriptorPool &) = delete;

    ~DescriptorPool();

    const bool allocateDescriptorSet(const VkDescriptorSetLayout descriptor_set_sayout,
                                     DescriptorSet &descriptor_set) const;

    void freeDescriptorSets(std::vector<DescriptorSet> &descriptor_sets) const;

    void resetPool();

  private:
    Device &device;
    VkDescriptorPool descriptorPool;

    friend class DescriptorWriter;
};
} // namespace vk
