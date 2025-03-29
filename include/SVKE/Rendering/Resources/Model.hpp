#pragma once

#include "SVKE/Core/System/Device.hpp"
#include "SVKE/Core/Graphics/Vertex.hpp"
#include "SVKE/Utils/HashCombine.hpp"
#include "SVKE/Core/System/Memory/Buffer.hpp"

#include <vk_mem_alloc.h>
#include <tiny_obj_loader.h>

#ifndef GLM_ENABLE_EXPERIMENTAL
#define GLM_ENABLE_EXPERIMENTAL
#endif
#include <glm/gtx/hash.hpp>

#include <cstring>
#include <memory>

namespace std
{
template <> struct hash<vk::Vertex>
{
    inline const size_t operator()(vk::Vertex const &vertex) const
    {
        size_t seed = 0;

        vk::hashCombine(seed, vertex.position, vertex.color, vertex.normal, vertex.uv);
        return seed;
    }
};
} // namespace std

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

    [[nodiscard]]
    const bool loadFromFile(const std::string &path);

    void bind(VkCommandBuffer &command_buffer);

    void draw(VkCommandBuffer &command_buffer);

    static std::unique_ptr<Model> createCubeModel(Device &device, const glm::vec3 &offset);

  private:
    Device &device;

    std::unique_ptr<Buffer> vertexBuffer;
    uint32_t vertexCount;

    std::unique_ptr<Buffer> indexBuffer;
    uint32_t indexCount;

    bool loaded;
    bool hasIndexBuffer;

    void createVertexBuffers(const VertexArray &vertices);
    void createIndexBuffers(const IndexArray &indices);
};

} // namespace vk
