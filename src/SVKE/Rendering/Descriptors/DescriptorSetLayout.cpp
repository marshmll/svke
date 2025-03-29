#include "SVKE/Rendering/Descriptors/DescriptorSetLayout.hpp"

vk::DescriptorSetLayout::Builder::Builder(Device &device) : device(device)
{
}

vk::DescriptorSetLayout::Builder &vk::DescriptorSetLayout::Builder::addBinding(const uint32_t binding,
                                                                               VkDescriptorType descriptor_type,
                                                                               VkShaderStageFlags stage_flags,
                                                                               const uint32_t count)
{
    assert(bindings.count(binding) == 0 && "BINDING ALREADY IN USE");

    VkDescriptorSetLayoutBinding layout_binding{};
    layout_binding.binding = binding;
    layout_binding.descriptorType = descriptor_type;
    layout_binding.descriptorCount = count;
    layout_binding.stageFlags = stage_flags;

    bindings[binding] = layout_binding;
    return *this;
}

std::unique_ptr<vk::DescriptorSetLayout> vk::DescriptorSetLayout::Builder::build() const
{
    return std::make_unique<DescriptorSetLayout>(device, bindings);
}

vk::DescriptorSetLayout::DescriptorSetLayout(Device &device,
                                             std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings)
    : device(device), bindings(bindings)
{
    std::vector<VkDescriptorSetLayoutBinding> descriptor_set_layout_bindings{};

    for (auto &[_, binding] : bindings)
        descriptor_set_layout_bindings.push_back(binding);

    VkDescriptorSetLayoutCreateInfo descriptor_set_layout_info{};
    descriptor_set_layout_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    descriptor_set_layout_info.bindingCount = static_cast<uint32_t>(descriptor_set_layout_bindings.size());
    descriptor_set_layout_info.pBindings = descriptor_set_layout_bindings.data();

    if (vkCreateDescriptorSetLayout(device.getLogicalDevice(), &descriptor_set_layout_info, nullptr,
                                    &descriptorSetLayout) != VK_SUCCESS)
        throw std::runtime_error(
            "vk::DescriptorSetLayout::DescriptorSetLayout: FAILED TO CREATE DESCRIPTOR SET LAYOUT");
}

vk::DescriptorSetLayout::~DescriptorSetLayout()
{
    vkDestroyDescriptorSetLayout(device.getLogicalDevice(), descriptorSetLayout, nullptr);
}

VkDescriptorSetLayout vk::DescriptorSetLayout::getDescriptorSetLayout() const
{
    return descriptorSetLayout;
}
