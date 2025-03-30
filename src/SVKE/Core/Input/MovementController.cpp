#include "SVKE/Core/Input/MovementController.hpp"

vk::MovementController::MovementController(Keyboard &keyboard, Mouse &mouse)
    : keyboard(keyboard), mouse(mouse), speed(1.5f), sensitivity(.003f)
{
}

void vk::MovementController::moveInPlaneXZ(const float dt, Object &object)
{
    Vec3f rotate{0.f};
    auto mouse_data = mouse.getCursorData();

    if (mouse.isRawModeEnabled())
    {
        // Apply raw deltas directly (no dt scaling for rotation)
        rotate.y += mouse_data.deltaX * sensitivity;  // Horizontal (yaw)
        rotate.x += -mouse_data.deltaY * sensitivity; // Vertical (pitch)
    }
    else
    {
        // Fallback for Non-Raw Input
        rotate.y += mouse_data.deltaX * sensitivity * dt * 60.0f; // Scale to 60Hz
        rotate.x += -mouse_data.deltaY * sensitivity * dt * 60.0f;
    }

    // Apply rotation
    if (Vector::dot(rotate, rotate) > std::numeric_limits<float>::epsilon())
        object.setRotation(object.getRotation() + rotate);

    // Clamp pitch and wrap yaw
    auto r = object.getRotation();
    r.x = glm::clamp(r.x, -1.5f, 1.5f);        // ~±85° pitch limit
    r.y = glm::mod(r.y, glm::two_pi<float>()); // Wrap yaw to [0, 2π]
    object.setRotation(r);

    // Movement (unchanged, still uses dt)
    float yaw = object.getRotation().y;
    const Vec3f forwardDir{sin(yaw), 0.f, cos(yaw)};
    const Vec3f rightDir{forwardDir.z, 0.f, -forwardDir.x};
    const Vec3f upDir{0.f, -1.f, 0.f};

    Vec3f moveDir{0.f};
    if (keyboard.isKeyPressed(keyMappings.moveForward))
        moveDir += forwardDir;
    if (keyboard.isKeyPressed(keyMappings.moveBackward))
        moveDir -= forwardDir;
    if (keyboard.isKeyPressed(keyMappings.moveRight))
        moveDir += rightDir;
    if (keyboard.isKeyPressed(keyMappings.moveLeft))
        moveDir -= rightDir;
    if (keyboard.isKeyPressed(keyMappings.moveUp))
        moveDir += upDir;
    if (keyboard.isKeyPressed(keyMappings.moveDown))
        moveDir -= upDir;

    if (Vector::dot(moveDir, moveDir) > std::numeric_limits<float>::epsilon())
        object.setTranslation(object.getTranslation() + speed * dt * Vector::normalize(moveDir));
}
