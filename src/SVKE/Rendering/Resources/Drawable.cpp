#include "SVKE/Rendering/Resources/Drawable.hpp"

vk::Drawable::Drawable(const Color &color) : color(color)
{
}

vk::Drawable::Drawable(std::shared_ptr<Model> &model, const Color &color) : model(model), color(color)
{
}

void vk::Drawable::bind(VkCommandBuffer &command_buffer)
{
    if (!model)
        return;

    model->bind(command_buffer);
}

void vk::Drawable::draw(VkCommandBuffer &command_buffer)
{
    if (!model)
        return;

    model->draw(command_buffer);
}

glm::mat4 vk::Drawable::transform()
{
    return transformComponent.mat4();
}

const glm::vec3 &vk::Drawable::getTranslation() const
{
    return transformComponent.translation;
}

const glm::vec3 &vk::Drawable::getScale() const
{
    return transformComponent.scale;
}

const glm::vec3 &vk::Drawable::getRotation() const
{
    return transformComponent.rotation;
}

const vk::Color &vk::Drawable::getColor() const
{
    return color;
}

const std::shared_ptr<vk::Model> &vk::Drawable::getModel() const
{
    return model;
}

const vk::Drawable::TransformComponent &vk::Drawable::getTransformComponent() const
{
    return transformComponent;
}

void vk::Drawable::setModel(std::shared_ptr<Model> &model)
{
    this->model = model;
}

void vk::Drawable::setColor(const Color &color)
{
    this->color = color;
}

void vk::Drawable::setTranslation(const glm::vec3 &translation)
{
    transformComponent.translation = translation;
}

void vk::Drawable::setScale(const glm::vec3 &scale)
{
    transformComponent.scale = scale;
}

void vk::Drawable::setRotation(const glm::vec3 &rotation)
{
    transformComponent.rotation = rotation;
}
