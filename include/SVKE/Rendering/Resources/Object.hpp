#pragma once

#include "SVKE/Rendering/Resources/Model.hpp"
#include "SVKE/Core/Graphics/Color.hpp"

#ifndef GLM_FORCE_RADIANS
#define GLM_FORCE_RADIANS
#endif
#ifndef GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#endif
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <memory>

namespace vk
{
class Object
{
  public:
    // struct Transform2dComponent
    // {
    //     glm::vec2 translation{};
    //     glm::vec2 scale{1.f, 1.f};
    //     float rotation = 0.f;

    //     inline glm::mat2 mat2()
    //     {
    //         const float sine = glm::sin(rotation);
    //         const float cosine = glm::cos(rotation);
    //         glm::mat2 rot_mat{{cosine, sine}, {-sine, cosine}};

    //         glm::mat2 scale_mat{{scale.x, 0.f}, {0.f, scale.y}};
    //         return rot_mat * scale_mat;
    //     }
    // };

    struct TransformComponent
    {
        glm::vec3 translation{};
        glm::vec3 scale{1.f, 1.f, 1.f};
        glm::vec3 rotation{};

        // Corresponds to: translate * Ry * Rx * Rz * scale transformation
        // Rotation convention uses Tail-Bryan angles with axis order Y(1), X(2), Z(3)
        // More: https://en.wikipedia.org/wiki/Euler_angles#Rotation_matrix
        inline glm::mat4 mat4()
        {
            const float c3 = glm::cos(rotation.z);
            const float s3 = glm::sin(rotation.z);
            const float c2 = glm::cos(rotation.x);
            const float s2 = glm::sin(rotation.x);
            const float c1 = glm::cos(rotation.y);
            const float s1 = glm::sin(rotation.y);
            return glm::mat4{{
                                 scale.x * (c1 * c3 + s1 * s2 * s3),
                                 scale.x * (c2 * s3),
                                 scale.x * (c1 * s2 * s3 - c3 * s1),
                                 0.0f,
                             },
                             {
                                 scale.y * (c3 * s1 * s2 - c1 * s3),
                                 scale.y * (c2 * c3),
                                 scale.y * (c1 * c3 * s2 + s1 * s3),
                                 0.0f,
                             },
                             {
                                 scale.z * (c2 * s1),
                                 scale.z * (-s2),
                                 scale.z * (c1 * c2),
                                 0.0f,
                             },
                             {translation.x, translation.y, translation.z, 1.0f}};
        }
    };

    Object(const Color &color = COLOR_WHITE);
    Object(std::shared_ptr<Model> &model, const Color &color = COLOR_WHITE);
    Object &operator=(const Object &) = delete;
    Object(Object &&) = default;
    Object &operator=(Object &&) = default;

    void bind(VkCommandBuffer &command_buffer);

    void draw(VkCommandBuffer &command_buffer);

    glm::mat4 transform();

    const Color &getColor() const;

    const std::shared_ptr<Model> &getModel() const;

    const TransformComponent &getTransformComponent() const;

    const glm::vec3 &getTranslation() const;

    const glm::vec3 &getScale() const;

    const glm::vec3 &getRotation() const;

    void setModel(std::shared_ptr<Model> &model);

    void setColor(const Color &color);

    void setTranslation(const glm::vec3 &translation);

    void setScale(const glm::vec3 &scale);

    void setRotation(const glm::vec3 &rotation);

  protected:
    std::shared_ptr<Model> model;
    Color color;
    TransformComponent transformComponent;
};
} // namespace vk
