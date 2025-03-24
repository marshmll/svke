#include "Graphics/Rendering/Drawable.hpp"

fl::Drawable::Drawable() : color({1.f, 1.f, 1.f})
{
}

fl::Drawable::Drawable(std::shared_ptr<Model> &model) : model(model), color({1.f, 1.f, 1.f})
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

const glm::vec3 &fl::Drawable::getColor() const
{
    return color;
}

glm::mat2 fl::Drawable::transform()
{
    return transform2d.mat2();
}

const fl::Drawable::Transform2dComponent &fl::Drawable::getTransform2d() const
{
    return transform2d;
}

void fl::Drawable::setModel(std::shared_ptr<Model> &model)
{
    this->model = model;
}

void fl::Drawable::setColor(const glm::vec3 &color)
{
    this->color = color;
}

void fl::Drawable::translate(const glm::vec2 &translation)
{
    transform2d.translation = translation;
}

void fl::Drawable::setScale(const glm::vec2 &scale)
{
    transform2d.scale = scale;
}

void fl::Drawable::rotate(const float angle)
{
    transform2d.rotation = (angle / 360.f) * glm::two_pi<float>();
}
