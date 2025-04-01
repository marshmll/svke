#pragma once

#include "SVKE/Rendering/Resources/Model.hpp"
#include "SVKE/Core/Graphics/Color.hpp"
#include "SVKE/Core/Graphics/TextureImage.hpp"
#include "SVKE/Core/Math/Vector.hpp"
#include "SVKE/Core/Math/Matrix.hpp"

#include <memory>
#include <optional>

namespace vk
{
class Object
{
  public:
    using objid_t = uint32_t;
    using Map = std::unordered_map<objid_t, Object>;

    struct TransformComponent
    {
        Vec3f translation{};
        Vec3f scale{1.f, 1.f, 1.f};
        Vec3f rotation{};

        // Corresponds to: translate * Ry * Rx * Rz * scale transformation
        // Rotation convention uses Tail-Bryan angles with axis order Y(1), X(2), Z(3)
        // More: https://en.wikipedia.org/wiki/Euler_angles#Rotation_matrix
        Mat4f mat4();
        Mat3f normalMatrix();
    };

    struct PointLightComponent
    {
        float lightIntensity;
    };

    Object(const Color &color = COLOR_WHITE);
    Object(std::shared_ptr<Model> &model, const Color &color = COLOR_WHITE);
    Object &operator=(const Object &) = delete;
    Object(Object &&) = default;
    Object &operator=(Object &&) = default;

    void bind(VkCommandBuffer &command_buffer);

    void draw(VkCommandBuffer &command_buffer);

    Mat4f transform();

    Mat3f normalMatrix();

    const objid_t &getId() const;

    const Color &getColor() const;

    const std::shared_ptr<Model> &getModel() const;

    const std::shared_ptr<TextureImage> &getTextureImage() const;

    const TransformComponent &getTransformComponent() const;

    const std::optional<PointLightComponent> &getPointLightComponent() const;

    const Vec3f &getTranslation() const;

    const Vec3f &getScale() const;

    const Vec3f &getRotation() const;

    void setModel(std::shared_ptr<Model> &model);

    void setTextureImage(std::shared_ptr<TextureImage> &tex_image);

    void setColor(const Color &color);

    void setTranslation(const Vec3f &translation);

    void setScale(const Vec3f &scale);

    void setRotation(const Vec3f &rotation);

    void createPointLightComponent(const PointLightComponent &component);

    static Object makePointLight(const float intensity = 10.f, const float radius = 0.1f,
                                 const Color color = COLOR_WHITE);

  protected:
    objid_t id;
    Color color;
    TransformComponent transformComponent;

    // Optional
    std::shared_ptr<Model> model;
    std::shared_ptr<TextureImage> textureImage;
    std::optional<PointLightComponent> pointLightComponent;
};
} // namespace vk
