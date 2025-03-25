#pragma once

#include "Graphics/Core/Model.hpp"
#include "Graphics/Color/Color.hpp"

#ifndef GLM_FORCE_RADIANS
#define GLM_FORCE_RADIANS
#endif
#ifndef GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#endif
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include <memory>

namespace fl
{
class Drawable
{
  public:
    struct Transform2dComponent
    {
        glm::vec2 translation{};
        glm::vec2 scale{1.f, 1.f};
        float rotation = 0.f;

        inline glm::mat2 mat2()
        {
            const float sine = glm::sin(rotation);
            const float cosine = glm::cos(rotation);
            glm::mat2 rot_mat{{cosine, sine}, {-sine, cosine}};

            glm::mat2 scale_mat{{scale.x, 0.f}, {0.f, scale.y}};
            return rot_mat * scale_mat;
        }
    };

    Drawable(const Color &color = COLOR_WHITE);
    Drawable(std::shared_ptr<Model> &model, const Color &color = COLOR_WHITE);
    Drawable &operator=(const Drawable &) = delete;
    Drawable(Drawable &&) = default;
    Drawable &operator=(Drawable &&) = default;

    void bind(VkCommandBuffer &command_buffer);

    void draw(VkCommandBuffer &command_buffer);

    const Color &getColor() const;

    const Transform2dComponent &getTransform2d() const;

    glm::mat2 transform();

    void setModel(std::shared_ptr<Model> &model);

    void setColor(const Color &color);

    void translate(const glm::vec2 &translation);

    void setScale(const glm::vec2 &scale);

    void rotate(const float angle);

  protected:
    std::shared_ptr<Model> model;
    Color color;
    Transform2dComponent transform2d;
};
} // namespace fl
