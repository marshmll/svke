#include "SVKE/Rendering/Descriptors/DescriptorWriter.hpp"

vk::DescriptorWriter::DescriptorWriter(DescriptorSetLayout &set_layout, DescriptorPool &pool)
    : setLayout(set_layout), pool(pool)
{
}

vk::DescriptorWriter &vk::DescriptorWriter::writeBuffer(const uint32_t binding, VkDescriptorBufferInfo &buffer_info)
{
    assert(setLayout.bindings.count(binding) == 1 && "LAYOUT DOES NOT CONTAIN SPECIFIED BINDING");

    auto &bindingDescription = setLayout.bindings[binding];

    assert(bindingDescription.descriptorCount == 1 && "BINDING SINGLE DESCRIPTOR INFO, BUT BINDING EXPECTS MULTIPLE");

    VkWriteDescriptorSet write{};
    write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    write.descriptorType = bindingDescription.descriptorType;
    write.dstBinding = binding;
    write.pBufferInfo = &buffer_info;
    write.descriptorCount = 1;

    writes.push_back(write);
    return *this;
}

vk::DescriptorWriter &vk::DescriptorWriter::writeImage(const uint32_t binding, VkDescriptorImageInfo &image_info)
{

    assert(setLayout.bindings.count(binding) == 1 && "LAYOUT DOES NOT CONTAIN SPECIFIED BINDING");

    auto &bindingDescription = setLayout.bindings[binding];

    assert(bindingDescription.descriptorCount == 1 && "BINDING SINGLE DESCRIPTOR INFO, BUT BINDING EXPECTS MULTIPLE");

    VkWriteDescriptorSet write{};
    write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    write.descriptorType = bindingDescription.descriptorType;
    write.dstBinding = binding;
    write.pImageInfo = &image_info;
    write.descriptorCount = 1;

    writes.push_back(write);
    return *this;
}

const bool vk::DescriptorWriter::build(DescriptorSet &set)
{
    const bool success = pool.allocateDescriptorSet(setLayout.getDescriptorSetLayout(), set);

    if (!success)
        return false;

    overwrite(set);
    return true;
}

void vk::DescriptorWriter::overwrite(DescriptorSet &set)
{
    for (auto &write : writes)
        write.dstSet = set;

    vkUpdateDescriptorSets(pool.device.getLogicalDevice(), writes.size(), writes.data(), 0, nullptr);
}
