#pragma once

#ifndef GLFW_INCLUDE_VULKAN
#define GLFW_INCLUDE_VULKAN
#endif
#include <GLFW/glfw3.h>

#include <vk_mem_alloc.h>

#include "SVKE/Core/System/Device.hpp"

namespace vk
{
class Buffer
{
  public:
    Buffer(Device &device, VkDeviceSize size, VkBufferUsageFlags usage, VmaMemoryUsage memory_usage);

    Buffer(Device &device, VkDeviceSize size, VkBufferUsageFlags usage, VmaMemoryUsage memory_usage,
           VmaAllocationCreateFlags flags);

    Buffer(const Buffer &) = delete;
    Buffer &operator=(const Buffer &) = delete;
    Buffer(Buffer &&) = delete;
    Buffer &operator=(Buffer &&) = delete;

    ~Buffer();

    void map();

    void unmap();

    void write(void *data, VkDeviceSize size);

    void copyTo(Buffer &other, const VkDeviceSize &size);

    const VkDeviceSize &getSize() const;

    VkBuffer &getBuffer();

    VkDescriptorBufferInfo getDescriptorInfo(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);

  private:
    Device &device;
    VkBuffer buffer;
    VmaAllocation allocation;
    VkDeviceSize size;
    void *mappedMem;
};
} // namespace vk
