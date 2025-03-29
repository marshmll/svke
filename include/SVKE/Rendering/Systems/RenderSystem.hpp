#pragma once

#include "SVKE/Core/Graphics/Pipeline.hpp"
#include "SVKE/Core/System/Device.hpp"
#include "SVKE/Core/System/Memory/Alignment.hpp"
#include "SVKE/Rendering/Camera.hpp"
#include "SVKE/Rendering/FrameInfo.hpp"
#include "SVKE/Rendering/Resources/Object.hpp"
#include "SVKE/Rendering/Systems/Renderer.hpp"
#include "SVKE/Rendering/Descriptors/DescriptorSetLayout.hpp"

#ifndef GLM_FORCE_RADIANS
#define GLM_FORCE_RADIANS
#endif
#ifndef GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#endif
#include <glm/glm.hpp>

#include <array>

namespace vk
{
class RenderSystem
{
    struct PushConstantData
    {
        ALIGNAS_MAT4 glm::mat4 modelMatrix{1.f};
        ALIGNAS_MAT4 glm::mat4 normalMatrix{1.f};
    };

  public:
    RenderSystem(Device &device, Renderer &renderer, DescriptorSetLayout &global_set_layout);
    RenderSystem(const RenderSystem &) = delete;
    RenderSystem &operator=(const RenderSystem &) = delete;

    ~RenderSystem();

    void render(const FrameInfo &frame_info, std::vector<Object> &objects);

  private:
    Device &device;

    VkPipelineLayout pipelineLayout;
    std::unique_ptr<Pipeline> pipeline;

    std::unique_ptr<Shader> vertShader;
    std::unique_ptr<Shader> fragShader;

    void createDevice();

    void loadShaders();

    void createPipelineLayout(DescriptorSetLayout &global_set_layout);

    void createPipeline(VkRenderPass render_pass);
};
} // namespace vk
