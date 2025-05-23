#include "SVKE/Rendering/Resources/Object.hpp"

vk::Object::Object(const Color &color) : color(color)
{
    static objid_t GLOBAL_ID_COUNTER = 0;

    id = GLOBAL_ID_COUNTER++;
}

vk::Object::Object(std::shared_ptr<Model> &model, const Color &color) : model(model), color(color)
{
    static objid_t GLOBAL_ID_COUNTER = 0;

    id = GLOBAL_ID_COUNTER++;
}

void vk::Object::bind(VkCommandBuffer &command_buffer)
{
    if (!model)
        return;

    model->bind(command_buffer);
}

void vk::Object::draw(VkCommandBuffer &command_buffer)
{
    if (!model)
        return;

    model->draw(command_buffer);
}

const std::shared_ptr<vk::TextureImage> &vk::Object::getTextureImage() const
{
    return textureImage;
}

vk::Mat4f vk::Object::transform()
{
    return transformComponent.mat4();
}

vk::Mat3f vk::Object::normalMatrix()
{
    return transformComponent.normalMatrix();
}

const std::optional<vk::Object::PointLightComponent> &vk::Object::getPointLightComponent() const
{
    return pointLightComponent;
}

const vk::Object::objid_t &vk::Object::getId() const
{
    return id;
}

const vk::Vec3f &vk::Object::getTranslation() const
{
    return transformComponent.translation;
}

const vk::Vec3f &vk::Object::getScale() const
{
    return transformComponent.scale;
}

const vk::Vec3f &vk::Object::getRotation() const
{
    return transformComponent.rotation;
}

const vk::Color &vk::Object::getColor() const
{
    return color;
}

const std::shared_ptr<vk::Model> &vk::Object::getModel() const
{
    return model;
}

const vk::Object::TransformComponent &vk::Object::getTransformComponent() const
{
    return transformComponent;
}

void vk::Object::setModel(std::shared_ptr<Model> &model)
{
    this->model = model;
}

void vk::Object::setTextureImage(std::shared_ptr<TextureImage> &tex_image)
{
    textureImage = tex_image;
}

void vk::Object::setColor(const Color &color)
{
    this->color = color;
}

void vk::Object::setTranslation(const Vec3f &translation)
{
    transformComponent.translation = translation;
}

void vk::Object::setScale(const Vec3f &scale)
{
    transformComponent.scale = scale;
}

void vk::Object::setRotation(const Vec3f &rotation)
{
    transformComponent.rotation = rotation;
}

void vk::Object::createPointLightComponent(const PointLightComponent &component)
{
    pointLightComponent = component;
}

vk::Object vk::Object::makePointLight(const float intensity, const float radius, const Color color)
{
    Object point_light;
    point_light.setColor(color);
    point_light.setScale({radius, 1.f, 1.f});
    point_light.createPointLightComponent({intensity});

    return point_light;
}

vk::Mat4f vk::Object::TransformComponent::mat4()
{
    const float c3 = glm::cos(rotation.z);
    const float s3 = glm::sin(rotation.z);
    const float c2 = glm::cos(rotation.x);
    const float s2 = glm::sin(rotation.x);
    const float c1 = glm::cos(rotation.y);
    const float s1 = glm::sin(rotation.y);
    return Mat4f{{
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

vk::Mat3f vk::Object::TransformComponent::normalMatrix()
{
    const float c3 = glm::cos(rotation.z);
    const float s3 = glm::sin(rotation.z);
    const float c2 = glm::cos(rotation.x);
    const float s2 = glm::sin(rotation.x);
    const float c1 = glm::cos(rotation.y);
    const float s1 = glm::sin(rotation.y);
    const Vec3f inverseScale = 1.0f / scale;

    return Mat3f{{
                     inverseScale.x * (c1 * c3 + s1 * s2 * s3),
                     inverseScale.x * (c2 * s3),
                     inverseScale.x * (c1 * s2 * s3 - c3 * s1),
                 },
                 {
                     inverseScale.y * (c3 * s1 * s2 - c1 * s3),
                     inverseScale.y * (c2 * c3),
                     inverseScale.y * (c1 * c3 * s2 + s1 * s3),
                 },
                 {
                     inverseScale.z * (c2 * s1),
                     inverseScale.z * (-s2),
                     inverseScale.z * (c1 * c2),
                 }};
}
