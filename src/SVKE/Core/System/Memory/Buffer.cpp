#include "SVKE/Core/System/Memory/Buffer.hpp"

vk::Buffer::Buffer(Device &device, VkDeviceSize size, VkBufferUsageFlags usage, VmaMemoryUsage memory_usage)
    : device(device), buffer(VK_NULL_HANDLE), allocation(VK_NULL_HANDLE), size(0), mappedMem(nullptr)
{
    VkBufferCreateInfo buffer_info = {};
    buffer_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    buffer_info.size = size;
    buffer_info.usage = usage;

    VmaAllocationCreateInfo alloc_info = {};
    alloc_info.usage = memory_usage;

    if (vmaCreateBuffer(device.getAllocator(), &buffer_info, &alloc_info, &buffer, &allocation, nullptr) != VK_SUCCESS)
        throw std::runtime_error("vk::Buffer::Buffer: FAILED TO CREATE BUFFER");
}

vk::Buffer::Buffer(Device &device, VkDeviceSize size, VkBufferUsageFlags usage, VmaMemoryUsage memory_usage,
                   VmaAllocationCreateFlags flags)
    : device(device), buffer(VK_NULL_HANDLE), allocation(VK_NULL_HANDLE), size(0), mappedMem(nullptr)
{
    VkBufferCreateInfo buffer_info = {};
    buffer_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    buffer_info.size = size;
    buffer_info.usage = usage;

    VmaAllocationCreateInfo alloc_info = {};
    alloc_info.usage = memory_usage;
    alloc_info.flags = flags;

    if (vmaCreateBuffer(device.getAllocator(), &buffer_info, &alloc_info, &buffer, &allocation, nullptr) != VK_SUCCESS)
        throw std::runtime_error("vk::Buffer::Buffer: FAILED TO CREATE BUFFER");
}

vk::Buffer::~Buffer()
{
    vkDeviceWaitIdle(device.getLogicalDevice());
    vmaDestroyBuffer(device.getAllocator(), buffer, allocation);
}

void vk::Buffer::map()
{
    assert(mappedMem == nullptr && "CANNOT MAP ALREADY MAPPED BUFFER");

    vmaMapMemory(device.getAllocator(), allocation, &mappedMem);
}

void vk::Buffer::unmap()
{
    assert(mappedMem != nullptr && "CANNOT UNMAP NOT MAPPED BUFFER");

    vmaUnmapMemory(device.getAllocator(), allocation);

    mappedMem = nullptr; // Reset pointer
}

void vk::Buffer::write(void *data, VkDeviceSize size)
{
    assert(mappedMem != nullptr && "CANNOT WRITE TO NOT MAPPED BUFFER");

    memcpy(mappedMem, data, size);
}

const VkDeviceSize &vk::Buffer::getSize() const
{
    return size;
}

VkBuffer &vk::Buffer::getBuffer()
{
    return buffer;
}

void vk::Buffer::copyTo(Buffer &other, const VkDeviceSize &size)
{
    VkCommandBuffer command_buffer = device.beginSingleTimeCommands();

    VkBufferCopy copy_region = {};
    copy_region.srcOffset = 0;
    copy_region.dstOffset = 0;
    copy_region.size = size;
    vkCmdCopyBuffer(command_buffer, this->buffer, other.getBuffer(), 1, &copy_region);

    device.endSingleTimeCommands(command_buffer);
}
