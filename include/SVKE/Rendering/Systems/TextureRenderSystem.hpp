#pragma once

#include "SVKE/Core/Graphics/Pipeline.hpp"
#include "SVKE/Core/System/Device.hpp"
#include "SVKE/Core/System/Memory/Alignment.hpp"
#include "SVKE/Rendering/Camera.hpp"
#include "SVKE/Rendering/FrameInfo.hpp"
#include "SVKE/Rendering/Resources/Object.hpp"
#include "SVKE/Rendering/Systems/Renderer.hpp"
#include "SVKE/Rendering/Descriptors/DescriptorSetLayout.hpp"

#include <array>

namespace vk
{
class TextureRenderSystem
{
    struct PushConstantData
    {
        ALIGNAS_MAT4 Mat4f modelMatrix{1.f};
        ALIGNAS_MAT4 Mat4f normalMatrix{1.f};
    };

  public:
    TextureRenderSystem(Device &device, Renderer &renderer, std::vector<VkDescriptorSetLayout> &set_layouts);
    TextureRenderSystem(const TextureRenderSystem &) = delete;
    TextureRenderSystem &operator=(const TextureRenderSystem &) = delete;

    ~TextureRenderSystem();

    void render(const FrameInfo &frame_info);

  private:
    Device &device;

    VkPipelineLayout pipelineLayout;
    std::unique_ptr<Pipeline> pipeline;

    std::unique_ptr<Shader> vertShader;
    std::unique_ptr<Shader> fragShader;

    void loadShaders();

    void createPipelineLayout(std::vector<VkDescriptorSetLayout> &set_layouts);

    void createPipeline(VkRenderPass render_pass);
};
} // namespace vk
