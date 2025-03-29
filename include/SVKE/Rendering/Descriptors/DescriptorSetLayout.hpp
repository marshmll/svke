#pragma once

#include "SVKE/Core/System/Device.hpp"
#include "SVKE/Rendering/Descriptors/DescriptorSet.hpp"

#include <memory>
#include <unordered_map>

namespace vk
{
class DescriptorSetLayout
{
  public:
    class Builder
    {
      public:
        Builder(Device &device);

        Builder &addBinding(const uint32_t binding, VkDescriptorType descriptor_type, VkShaderStageFlags stage_flags,
                            const uint32_t count = 1);

        std::unique_ptr<DescriptorSetLayout> build() const;

      private:
        Device &device;
        std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings;
    };

    DescriptorSetLayout(Device &device, std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings);

    DescriptorSetLayout(const DescriptorSetLayout &) = delete;
    DescriptorSetLayout &operator=(const DescriptorSetLayout &) = delete;

    ~DescriptorSetLayout();

    VkDescriptorSetLayout getDescriptorSetLayout() const;

  private:
    Device &device;
    VkDescriptorSetLayout descriptorSetLayout;
    std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings;

    friend class DescriptorWriter;
};
} // namespace vk
