#include "SVKE/Rendering/Descriptors/DescriptorPool.hpp"

vk::DescriptorPool::Builder::Builder(Device &device) : device(device), maxSets(1000), poolFlags(0)
{
}

vk::DescriptorPool::Builder &vk::DescriptorPool::Builder::addPoolSize(VkDescriptorType descriptor_type,
                                                                      const uint32_t count)
{
    poolSizes.push_back({descriptor_type, count});
    return *this;
}

vk::DescriptorPool::Builder &vk::DescriptorPool::Builder::setPoolFlags(VkDescriptorPoolCreateFlags flags)
{
    poolFlags = flags;
    return *this;
}

vk::DescriptorPool::Builder &vk::DescriptorPool::Builder::setMaxSets(const uint32_t count)
{
    maxSets = count;
    return *this;
}

std::unique_ptr<vk::DescriptorPool> vk::DescriptorPool::Builder::build() const
{
    return std::make_unique<DescriptorPool>(device, maxSets, poolFlags, poolSizes);
}

vk::DescriptorPool::DescriptorPool(Device &device, const uint32_t max_sets, VkDescriptorPoolCreateFlags pool_flags,
                                   const std::vector<VkDescriptorPoolSize> &pool_sizes)
    : device(device)
{
    VkDescriptorPoolCreateInfo descriptor_pool_info{};
    descriptor_pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    descriptor_pool_info.poolSizeCount = static_cast<uint32_t>(pool_sizes.size());
    descriptor_pool_info.pPoolSizes = pool_sizes.data();
    descriptor_pool_info.maxSets = max_sets;
    descriptor_pool_info.flags = pool_flags;

    if (vkCreateDescriptorPool(device.getLogicalDevice(), &descriptor_pool_info, nullptr, &descriptorPool) !=
        VK_SUCCESS)
        throw std::runtime_error("vk::DescriptorPool::DescriptorPool: FAILED TO CREATE DESCRIPTOR POOL");
}

vk::DescriptorPool::~DescriptorPool()
{
    vkDestroyDescriptorPool(device.getLogicalDevice(), descriptorPool, nullptr);
}

const bool vk::DescriptorPool::allocateDescriptorSet(const VkDescriptorSetLayout descriptor_set_sayout,
                                                     DescriptorSet &descriptor_set) const
{
    VkDescriptorSetAllocateInfo alloc_info{};
    alloc_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    alloc_info.descriptorPool = descriptorPool;
    alloc_info.pSetLayouts = &descriptor_set_sayout;
    alloc_info.descriptorSetCount = 1;

    // Might want to create a "DescriptorPoolManager" class that handles this case, and builds
    // a new pool whenever an old pool fills up.
    if (vkAllocateDescriptorSets(device.getLogicalDevice(), &alloc_info, &descriptor_set) != VK_SUCCESS)
        return false;

    return true;
}

void vk::DescriptorPool::freeDescriptorSets(std::vector<DescriptorSet> &descriptor_sets) const
{
    vkFreeDescriptorSets(device.getLogicalDevice(), descriptorPool, static_cast<uint32_t>(descriptor_sets.size()),
                         descriptor_sets.data());
}

void vk::DescriptorPool::resetPool()
{
    vkResetDescriptorPool(device.getLogicalDevice(), descriptorPool, 0);
}
