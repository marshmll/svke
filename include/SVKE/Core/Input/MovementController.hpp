#pragma once

#include "SVKE/Core/Input/Keyboard.hpp"
#include "SVKE/Core/Input/Mouse.hpp"
#include "SVKE/Rendering/Resources/Object.hpp"

namespace vk
{
class MovementController
{
  public:
    struct KeyMappings
    {
        Keyboard::Key moveForward = Keyboard::Key::W;
        Keyboard::Key moveBackward = Keyboard::Key::S;
        Keyboard::Key moveLeft = Keyboard::Key::A;
        Keyboard::Key moveRight = Keyboard::Key::D;
        Keyboard::Key moveUp = Keyboard::Key::Space;
        Keyboard::Key moveDown = Keyboard::Key::LeftShift;
        Keyboard::Key lookUp = Keyboard::Key::Up;
        Keyboard::Key lookDown = Keyboard::Key::Down;
        Keyboard::Key lookLeft = Keyboard::Key::Left;
        Keyboard::Key lookRight = Keyboard::Key::Right;
    };

    MovementController(Keyboard &keyboard, Mouse &mouse);

    void moveInPlaneXZ(const float dt, Object &object);

  private:
    Keyboard &keyboard;
    Mouse &mouse;

    KeyMappings keyMappings;
    float speed;
    float sensitivity;
};
} // namespace vk
