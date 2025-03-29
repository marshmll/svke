#pragma once

#include "SVKE/Rendering/Camera.hpp"
#include "SVKE/Rendering/Resources/Object.hpp"
#include "SVKE/Core/System/Device.hpp"
#include "SVKE/Core/System/Pipeline.hpp"
#include "SVKE/Rendering/Systems/Renderer.hpp"
#include "SVKE/Core/System/Memory/Alignment.hpp"

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
        ALIGNAS_MAT4 glm::mat4 transform{1.f};
        ALIGNAS_MAT4 glm::mat4 normalMatrix{1.f};
    };

  public:
    RenderSystem(Device &device, Renderer &renderer);
    RenderSystem(const RenderSystem &) = delete;
    RenderSystem &operator=(const RenderSystem &) = delete;

    ~RenderSystem();

    void render(VkCommandBuffer &command_buffer, std::vector<Object> &objects, const Camera &camera);

  private:
    Device &device;

    VkPipelineLayout pipelineLayout;
    std::unique_ptr<Pipeline> pipeline;

    std::unique_ptr<Shader> vertShader;
    std::unique_ptr<Shader> fragShader;

    void createDevice();

    void loadShaders();

    void createPipelineLayout();

    void createPipeline(VkRenderPass render_pass);
};
} // namespace vk
