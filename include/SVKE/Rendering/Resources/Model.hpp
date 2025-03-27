#pragma once

#include "SVKE/Core/System/Device.hpp"
#include "SVKE/Core/Graphics/Vertex.hpp"

#include <vk_mem_alloc.h>

#include <cstring>
#include <memory>

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

    static std::unique_ptr<Model> createCubeModel(Device &device, const glm::vec3 &offset);
  private:
    Device &device;

    VkBuffer vertexBuffer;
    VmaAllocation vertexBufferMemory;
    uint32_t vertexCount;

    bool loaded;

    void createVertexBuffers(const VertexArray &vertices);
};

} // namespace fl
