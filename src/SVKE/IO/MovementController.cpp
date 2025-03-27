#include "SVKE/IO/MovementController.hpp"

vk::MovementController::MovementController(Keyboard &keyboard, Mouse &mouse)
    : keyboard(keyboard), mouse(mouse), speed(1.f), sensitivity(2.f)
{
    mouse.resetCursorData();
}

void vk::MovementController::moveInPlaneXZ(const float dt, Object &object)
{
    glm::vec3 rotate{0};
    auto mouse_data = mouse.getCursorData();

    if (mouse.getCursorMode() == Mouse::CursorMode::Disabled)
    {
        if (mouse_data.deltaX > 0.f)
            rotate.y += 1.f;

        if (mouse_data.deltaX < 0.f)
            rotate.y -= 1.f;

        if (mouse_data.deltaY < 0.f)
            rotate.x += 1.f;

        if (mouse_data.deltaY > 0.f)
            rotate.x -= 1.f;
    }

    auto mouse_speed = sensitivity + std::fabs(mouse_data.deltaX + mouse_data.deltaY) / 2.f;

    if (glm::dot(rotate, rotate) > std::numeric_limits<float>::epsilon())
        object.setRotation(object.getRotation() + mouse_speed * dt * glm::normalize(rotate));

    // limit pitch values between about +/- 85ish degrees
    auto r = object.getRotation();
    r.x = glm::clamp(r.x, -1.5f, 1.5f);
    r.y = glm::mod(r.y, glm::two_pi<float>());
    object.setRotation(r);

    float yaw = object.getRotation().y;
    const glm::vec3 forwardDir{sin(yaw), 0.f, cos(yaw)};
    const glm::vec3 rightDir{forwardDir.z, 0.f, -forwardDir.x};
    const glm::vec3 upDir{0.f, -1.f, 0.f};

    glm::vec3 moveDir{0.f};
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

    if (glm::dot(moveDir, moveDir) > std::numeric_limits<float>::epsilon())
        object.setTranslation(object.getTranslation() + speed * dt * glm::normalize(moveDir));
}
