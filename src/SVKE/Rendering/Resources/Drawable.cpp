#include "SVKE/Rendering/Resources/Drawable.hpp"

fl::Drawable::Drawable(const Color &color) : color(color)
{
}

fl::Drawable::Drawable(std::shared_ptr<Model> &model, const Color &color) : model(model), color(color)
{
}

void fl::Drawable::bind(VkCommandBuffer &command_buffer)
{
    if (!model)
        return;

    model->bind(command_buffer);
}

void fl::Drawable::draw(VkCommandBuffer &command_buffer)
{
    if (!model)
        return;

    model->draw(command_buffer);
}

glm::mat4 fl::Drawable::transform()
{
    return transformComponent.mat4();
}

const glm::vec3 &fl::Drawable::getTranslation() const
{
    return transformComponent.translation;
}

const glm::vec3 &fl::Drawable::getScale() const
{
    return transformComponent.scale;
}

const glm::vec3 &fl::Drawable::getRotation() const
{
    return transformComponent.rotation;
}

const fl::Color &fl::Drawable::getColor() const
{
    return color;
}

const std::shared_ptr<fl::Model> &fl::Drawable::getModel() const
{
    return model;
}

const fl::Drawable::TransformComponent &fl::Drawable::getTransformComponent() const
{
    return transformComponent;
}

void fl::Drawable::setModel(std::shared_ptr<Model> &model)
{
    this->model = model;
}

void fl::Drawable::setColor(const Color &color)
{
    this->color = color;
}

void fl::Drawable::setTranslation(const glm::vec3 &translation)
{
    transformComponent.translation = translation;
}

void fl::Drawable::setScale(const glm::vec3 &scale)
{
    transformComponent.scale = scale;
}

void fl::Drawable::setRotation(const glm::vec3 &rotation)
{
    transformComponent.rotation = rotation;
}
