#include "SVKE/IO/KeyboardMovementController.hpp"

vk::KeyboardMovementController::KeyboardMovementController(Keyboard &kb) : kb(kb), speed(1.f), sensitivity(1.5f)
{
}

void vk::KeyboardMovementController::moveInPlaneXZ(const float dt, Object &object)
{
    glm::vec3 rotate{0};
    if (kb.isKeyPressed(keyMappings.lookRight))
        rotate.y += 1.f;
    if (kb.isKeyPressed(keyMappings.lookLeft))
        rotate.y -= 1.f;
    if (kb.isKeyPressed(keyMappings.lookUp))
        rotate.x += 1.f;
    if (kb.isKeyPressed(keyMappings.lookDown))
        rotate.x -= 1.f;

    if (glm::dot(rotate, rotate) > std::numeric_limits<float>::epsilon())
        object.setRotation(object.getRotation() + sensitivity * dt * glm::normalize(rotate));

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
    if (kb.isKeyPressed(keyMappings.moveForward))
        moveDir += forwardDir;
    if (kb.isKeyPressed(keyMappings.moveBackward))
        moveDir -= forwardDir;
    if (kb.isKeyPressed(keyMappings.moveRight))
        moveDir += rightDir;
    if (kb.isKeyPressed(keyMappings.moveLeft))
        moveDir -= rightDir;
    if (kb.isKeyPressed(keyMappings.moveUp))
        moveDir += upDir;
    if (kb.isKeyPressed(keyMappings.moveDown))
        moveDir -= upDir;

    if (glm::dot(moveDir, moveDir) > std::numeric_limits<float>::epsilon())
            object.setTranslation(object.getTranslation() + speed * dt * glm::normalize(moveDir));
}
