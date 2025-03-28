#pragma once

#include "SVKE/Core/System/Device.hpp"
#include "SVKE/Core/Graphics/Vertex.hpp"

#include <vk_mem_alloc.h>
#include <tiny_obj_loader.h>

#include <cstring>
#include <memory>

namespace vk
{
class Model
{
  public:
    Model(Device &device);
    Model(const Model &) = delete;
    Model &operator=(const Model &) = delete;

    ~Model();

    void loadFromData(const VertexArray &vertices);

    void loadFromData(const VertexArray &vertices, const IndexArray &indices);

    void loadFromFile(const std::string &path);

    void bind(VkCommandBuffer &command_buffer);

    void draw(VkCommandBuffer &command_buffer);

    static std::unique_ptr<Model> createCubeModel(Device &device, const glm::vec3 &offset);

  private:
    Device &device;

    VkBuffer vertexBuffer;
    VmaAllocation vertexBufferMemory;
    uint32_t vertexCount;

    VkBuffer indexBuffer;
    VmaAllocation indexBufferMemory;
    uint32_t indexCount;

    bool loaded;
    bool hasIndexBuffer;

    void createVertexBuffers(const VertexArray &vertices);
    void createIndexBuffers(const IndexArray &indices);
};

} // namespace vk
