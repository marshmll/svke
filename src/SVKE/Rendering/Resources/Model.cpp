#include "SVKE/Rendering/Resources/Model.hpp"

vk::Model::Model(Device &device) : device(device), vertexCount(0), loaded(false), hasIndexBuffer(false)
{
}

vk::Model::~Model()
{
}

void vk::Model::loadFromData(const VertexArray &vertices)
{
    loaded = true;
    hasIndexBuffer = false;
    createVertexBuffers(vertices);
}

void vk::Model::loadFromData(const VertexArray &vertices, const IndexArray &indices)
{
    loaded = true;
    hasIndexBuffer = true;
    createVertexBuffers(vertices);
    createIndexBuffers(indices);
}

const bool vk::Model::loadFromFile(const std::string &path)
{
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string warn, err;

    if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, path.c_str()))
    {
        std::cerr << "vk::Model::loadFromFile: FAILED TO LOAD MODEL FROM FILE: " << path << " : " << warn << " " << err;
        return false;
    }

    VertexArray vertices;
    IndexArray indices;
    std::unordered_map<Vertex, Index> unique_vertices;

    for (auto &shape : shapes)
    {
        for (auto &index : shape.mesh.indices)
        {
            Vertex v{};

            if (index.vertex_index >= 0)
            {
                v.position = {
                    attrib.vertices[3 * index.vertex_index + 0],
                    attrib.vertices[3 * index.vertex_index + 1],
                    attrib.vertices[3 * index.vertex_index + 2],
                };

                v.color = {
                    attrib.colors[3 * index.vertex_index + 0],
                    attrib.colors[3 * index.vertex_index + 1],
                    attrib.colors[3 * index.vertex_index + 2],
                };
            }

            if (index.normal_index >= 0)
            {
                v.normal = {attrib.normals[3 * index.normal_index + 0], attrib.normals[3 * index.normal_index + 1],
                            attrib.normals[3 * index.normal_index + 2]};
            }

            if (index.texcoord_index >= 0)
            {
                v.uv = {attrib.texcoords[2 * index.texcoord_index + 0],
                        1.0f - attrib.texcoords[2 * index.texcoord_index + 1]};
            }

            if (unique_vertices.find(v) == unique_vertices.end())
            {
                unique_vertices[v] = static_cast<Index>(vertices.size());
                vertices.emplace_back(v);
            }

            indices.push_back(unique_vertices[v]);
        }
    }

    if (indices.size() > 0)
        loadFromData(vertices, indices);
    else
        loadFromData(vertices);

#ifndef NDEBUG
    std::cout << "LOADED MODEL (" << vertices.size() << " VERTICES, " << indices.size()
              << " INDICES FROM FILE: " << path << std::endl;
#endif

    return true;
}

void vk::Model::bind(VkCommandBuffer &command_buffer)
{
    assert(loaded == true && "CANNOT BIND UNINITIALIZED MODEL");

    VkBuffer buffers[] = {vertexBuffer->getBuffer()};
    VkDeviceSize offsets[] = {0};

    vkCmdBindVertexBuffers(command_buffer, 0, 1, buffers, offsets);

    if (hasIndexBuffer)
        vkCmdBindIndexBuffer(command_buffer, indexBuffer->getBuffer(), 0, VK_INDEX_TYPE_UINT32);
}

void vk::Model::draw(VkCommandBuffer &command_buffer)
{
    assert(loaded == true && "CANNOT DRAW UNINITIALIZED MODEL");

    if (hasIndexBuffer)
        vkCmdDrawIndexed(command_buffer, indexCount, 1, 0, 0, 0);

    else
        vkCmdDraw(command_buffer, vertexCount, 1, 0, 0);
}

std::unique_ptr<vk::Model> vk::Model::createCubeModel(Device &device, const glm::vec3 &offset)
{
    VertexArray vertices = VertexArray{
        // Left face
        Vertex{{-0.5f, -0.5f, -0.5f}, {1.f, 1.f, 1.f}},
        Vertex{{-0.5f, 0.5f, 0.5f}, {1.f, 1.f, 1.f}},
        Vertex{{-0.5f, -0.5f, 0.5f}, {1.f, 1.f, 1.f}},
        Vertex{{-0.5f, 0.5f, -0.5f}, {1.f, 1.f, 1.f}},

        // Right face
        Vertex{{0.5f, -0.5f, -0.5f}, {.8f, .8f, .1f}},
        Vertex{{0.5f, 0.5f, 0.5f}, {.8f, .8f, .1f}},
        Vertex{{0.5f, -0.5f, 0.5f}, {.8f, .8f, .1f}},
        Vertex{{0.5f, 0.5f, -0.5f}, {.8f, .8f, .1f}},

        // Top face
        Vertex{{-0.5f, -0.5f, -0.5f}, {.9f, .6f, .1f}},
        Vertex{{0.5f, -0.5f, 0.5f}, {.9f, .6f, .1f}},
        Vertex{{-0.5f, -0.5f, 0.5f}, {.9f, .6f, .1f}},
        Vertex{{0.5f, -0.5f, -0.5f}, {.9f, .6f, .1f}},

        // Bottom face
        Vertex{{-0.5f, 0.5f, -0.5f}, {.8f, .1f, .1f}},
        Vertex{{0.5f, 0.5f, 0.5f}, {.8f, .1f, .1f}},
        Vertex{{-0.5f, 0.5f, 0.5f}, {.8f, .1f, .1f}},
        Vertex{{0.5f, 0.5f, -0.5f}, {.8f, .1f, .1f}},

        // Front face
        Vertex{{-0.5f, -0.5f, 0.5f}, {.1f, .1f, .8f}},
        Vertex{{0.5f, 0.5f, 0.5f}, {.1f, .1f, .8f}},
        Vertex{{-0.5f, 0.5f, 0.5f}, {.1f, .1f, .8f}},
        Vertex{{0.5f, -0.5f, 0.5f}, {.1f, .1f, .8f}},

        // Back face
        Vertex{{-0.5f, -0.5f, -0.5f}, {.1f, .8f, .1f}},
        Vertex{{0.5f, 0.5f, -0.5f}, {.1f, .8f, .1f}},
        Vertex{{-0.5f, 0.5f, -0.5f}, {.1f, .8f, .1f}},
        Vertex{{0.5f, -0.5f, -0.5f}, {.1f, .8f, .1f}},
    };

    IndexArray indices = {0,  1,  2,  0,  3,  1,  4,  5,  6,  4,  7,  5,  8,  9,  10, 8,  11, 9,
                          12, 13, 14, 12, 15, 13, 16, 17, 18, 16, 19, 17, 20, 21, 22, 20, 23, 21};

    for (auto &vertex : vertices)
        vertex.position += offset;

    auto model = std::make_unique<Model>(device);
    model->loadFromData(vertices, indices);

    return std::move(model);
}

void vk::Model::createVertexBuffers(const VertexArray &vertices)
{
    vertexCount = static_cast<uint32_t>(vertices.size());
    assert(vertexCount >= 3 && "VERTEX COUNT MUST BE AT LEAST 3");

    VkDeviceSize buffer_size = vertexCount * sizeof(Vertex);

    Buffer staging_buffer(device, buffer_size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VMA_MEMORY_USAGE_AUTO,
                          VMA_ALLOCATION_CREATE_HOST_ACCESS_ALLOW_TRANSFER_INSTEAD_BIT |
                              VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT);

    staging_buffer.map();
    staging_buffer.write((void *)vertices.data(), buffer_size);
    staging_buffer.unmap();

    vertexBuffer = std::make_unique<Buffer>(device, buffer_size,
                                            VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                                            VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE);

    staging_buffer.copyTo(*vertexBuffer, buffer_size);
}

void vk::Model::createIndexBuffers(const IndexArray &indices)
{
    indexCount = static_cast<uint32_t>(indices.size());
    assert(indexCount >= 3 && "INDEX COUNT MUST BE AT LEAST 3");

    VkDeviceSize buffer_size = indexCount * sizeof(Index);

    Buffer staging_buffer(device, buffer_size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VMA_MEMORY_USAGE_AUTO,
                          VMA_ALLOCATION_CREATE_HOST_ACCESS_ALLOW_TRANSFER_INSTEAD_BIT |
                              VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT);

    staging_buffer.map();
    staging_buffer.write((void *)indices.data(), buffer_size);
    staging_buffer.unmap();

    indexBuffer = std::make_unique<Buffer>(device, buffer_size,
                                           VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
                                           VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE);

    staging_buffer.copyTo(*indexBuffer, buffer_size);
}
