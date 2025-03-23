#pragma once

#include "System/Core/Device.hpp"
#include "Graphics/Core/Vertex.hpp"

#include <vk_mem_alloc.h>

#include <cstring>

namespace fl
{
class Model
{
  public:
    Model(Device &device);
    Model(const Model &) = delete;
    Model &operator=(const Model &) = delete;

    ~Model();

    void loadFromData(const VertexArray &vertices);

    void bind(VkCommandBuffer &command_buffer);

    void draw(VkCommandBuffer &command_buffer);

  private:
    Device &device;

    VkBuffer vertexBuffer;
    VmaAllocation vertexBufferMemory;
    uint32_t vertexCount;

    bool loaded;

    void createVertexBuffers(const VertexArray &vertices);
};

} // namespace fl
