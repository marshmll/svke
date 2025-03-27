#include "SVKE/Rendering/Resources/Object.hpp"

vk::Object::Object(const Color &color) : color(color)
{
}

vk::Object::Object(std::shared_ptr<Model> &model, const Color &color) : model(model), color(color)
{
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

glm::mat4 vk::Object::transform()
{
    return transformComponent.mat4();
}

const glm::vec3 &vk::Object::getTranslation() const
{
    return transformComponent.translation;
}

const glm::vec3 &vk::Object::getScale() const
{
    return transformComponent.scale;
}

const glm::vec3 &vk::Object::getRotation() const
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

void vk::Object::setColor(const Color &color)
{
    this->color = color;
}

void vk::Object::setTranslation(const glm::vec3 &translation)
{
    transformComponent.translation = translation;
}

void vk::Object::setScale(const glm::vec3 &scale)
{
    transformComponent.scale = scale;
}

void vk::Object::setRotation(const glm::vec3 &rotation)
{
    transformComponent.rotation = rotation;
}
