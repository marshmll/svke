#include "Graphics/Core/Model.hpp"

fl::Model::Model(Device &device) : device(device), vertexCount(0), loaded(false)
{
}

fl::Model::~Model()
{
    vkDeviceWaitIdle(device.getLogicalDevice());
    vmaDestroyBuffer(device.getAllocator(), vertexBuffer, vertexBufferMemory);
}

void fl::Model::loadFromData(const VertexArray &vertices)
{
    createVertexBuffers(vertices);
}

void fl::Model::bind(VkCommandBuffer &command_buffer)
{
    assert(loaded == true && "CANNOT BIND UNINITIALIZED MODEL");

    VkBuffer buffers[] = {vertexBuffer};
    VkDeviceSize offsets[] = {0};

    vkCmdBindVertexBuffers(command_buffer, 0, 1, buffers, offsets);
}

void fl::Model::draw(VkCommandBuffer &command_buffer)
{
    assert(loaded == true && "CANNOT DRAW UNINITIALIZED MODEL");

    vkCmdDraw(command_buffer, vertexCount, 1, 0, 0);
}

void fl::Model::createVertexBuffers(const VertexArray &vertices)
{
    vertexCount = static_cast<uint32_t>(vertices.size());
    assert(vertexCount >= 3 && "VERTEX COUNT MUST BE AT LEAST 3");

    VkDeviceSize buffer_size = vertexCount * sizeof(Vertex);

    VkBuffer staging_buffer;
    VmaAllocation staging_buffer_allocation;

    device.createBuffer(buffer_size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VMA_MEMORY_USAGE_AUTO_PREFER_HOST,
                        staging_buffer, staging_buffer_allocation,
                        VMA_ALLOCATION_CREATE_HOST_ACCESS_ALLOW_TRANSFER_INSTEAD_BIT |
                            VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT);

    // Fill the staging buffer with data

    void *data;
    vmaMapMemory(device.getAllocator(), staging_buffer_allocation, &data);
    memcpy(data, vertices.data(), buffer_size);
    vmaUnmapMemory(device.getAllocator(), staging_buffer_allocation);

    // Create a device buffer (device-local)
    device.createBuffer(buffer_size, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                        VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE, vertexBuffer, vertexBufferMemory);

    // Copy data from staging buffer to device buffer
    device.copyBuffer(staging_buffer, vertexBuffer, buffer_size);

    // Cleanup
    vmaDestroyBuffer(device.getAllocator(), staging_buffer, staging_buffer_allocation);

    loaded = true;
}
