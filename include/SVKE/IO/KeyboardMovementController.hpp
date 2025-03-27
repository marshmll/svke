#pragma once

#ifndef GLM_FORCE_RADIANS
#define GLM_FORCE_RADIANS
#endif
#ifndef GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#endif
#include <glm/glm.hpp>

#include "SVKE/IO/Keyboard.hpp"
#include "SVKE/Rendering/Resources/Object.hpp"

namespace vk
{
class KeyboardMovementController
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

    KeyboardMovementController(Keyboard &kb);

    void moveInPlaneXZ(const float dt, Object &object);

  private:
    Keyboard &kb;

    KeyMappings keyMappings;
    float speed;
    float sensitivity;
};
} // namespace vk
