#pragma once

#include "Graphics/Projection/Camera.hpp"
#include "Graphics/Rendering/Drawable.hpp"
#include "System/Core/Device.hpp"
#include "System/Core/Pipeline.hpp"
#include "System/Rendering/Renderer.hpp"
#include "System/Memory/Alignment.hpp"

#ifndef GLM_FORCE_RADIANS
#define GLM_FORCE_RADIANS
#endif
#ifndef GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#endif
#include <glm/glm.hpp>

#include <array>

namespace fl
{
class RenderSystem
{
    struct PushConstantData
    {
        ALIGNAS_MAT4 glm::mat4 transform{1.f};
        ALIGNAS_VEC3 glm::vec3 color;
    };

  public:
    RenderSystem(Device &device, Renderer &renderer);
    RenderSystem(const RenderSystem &) = delete;
    RenderSystem &operator=(const RenderSystem &) = delete;

    ~RenderSystem();

    void render(VkCommandBuffer &command_buffer, std::vector<Drawable> &drawables, const Camera &camera);

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
} // namespace fl
