#include "Game\Scripts\PlayerController.h"

#include <Core\window.h>
#include <Game\Entity.h>
#include <Game\physics_components.h>

#include <box2d\b2_body.h>

struct PlayerController::InputState
{
    bool strafeLeftPressed : 1;
    bool strafeRightPressed : 1;
    bool moveForwardPressed : 1;
    bool moveBackwardPressed : 1;

    InputState()
        : strafeLeftPressed(false)
        , strafeRightPressed(false)
        , moveForwardPressed(false)
        , moveBackwardPressed(false) {}
};

PlayerController::PlayerController(std::shared_ptr<AppWindow> w)
    : input(std::make_unique<InputState>())
    , window(w)
{

}

PlayerController::~PlayerController()
{
    input.reset(nullptr);
}

void PlayerController::OnKeyDown(WORD key, bool repeat)
{
    switch (key)
    {
        case INPUT_KEY_STRAFE_LEFT:
        {
            input->strafeLeftPressed = true;
        } break;

        case INPUT_KEY_STRAFE_RIGHT:
        {
            input->strafeRightPressed = true;
        } break;

        case INPUT_KEY_FORWARD:
        {
            input->moveForwardPressed = true;
        } break;

        case INPUT_KEY_BACKWARD:
        {
            input->moveBackwardPressed = true;
        } break;
    }
}

void PlayerController::OnKeyUp(WORD key)
{
    switch (key)
    {
        case INPUT_KEY_STRAFE_LEFT:
        {
            input->strafeLeftPressed = false;
        } break;

        case INPUT_KEY_STRAFE_RIGHT:
        {
            input->strafeRightPressed = false;
        } break;

        case INPUT_KEY_FORWARD:
        {
            input->moveForwardPressed = false;
        } break;

        case INPUT_KEY_BACKWARD:
        {
            input->moveBackwardPressed = false;
        } break;
    }
}

void PlayerController::OnBegin()
{
    cameraEntity = GetOwner()->world->FindEntityByTag("camera");
    //window->keyboard->RegisterKeyListener(this);
}

void PlayerController::OnFinish()
{
    //window->keyboard->UnregisterKeyListener(this);
}

void PlayerController::OnUpdate(float delta)
{
    vec2 movementInput(0.0f);

    if (input->strafeLeftPressed)
    {
        movementInput.x -= 1.0f;
    }

    if (input->strafeRightPressed)
    {
        movementInput.x += 1.0f;
    }

    if (input->moveForwardPressed)
    {
        movementInput.y += 1.0f;
    }

    if (input->moveBackwardPressed)
    {
        movementInput.y -= 1.0f;
    }

    if (glm::length(movementInput) > 0.0f)
    {
        //auto& playerTransform = GetOwner()->GetComponent<TransformComponent>();
        //auto& cameraTransform = cameraEntity->GetComponent<TransformComponent>();
        //
        //float cameraRot = glm::radians(cameraTransform.rotation + 90.0f);
        //float cameraRotSin = glm::sin(cameraRot);
        //float cameraRotCos = glm::cos(cameraRot);
        //
        //vec2 forward = glm::normalize(movementInput);
        //vec2 movement = vec2(
        //    forward.x * cameraRotSin - forward.y * cameraRotCos
        //    , forward.x * cameraRotCos + forward.y * cameraRotSin);
        //
        //vec3 frame_movement = vec3(movement * MAX_MOVE_SPEED, 0.0f);
        //auto& rb = owner->GetComponent<RigidBodyComponent>();
        //
        //b2Body* body = (b2Body*)rb.runtime_body;
        //body->ApplyForceToCenter(b2Vec2(frame_movement.x, frame_movement.y), true);
        //
        //// the player has a fixed rotation so we can set this however we like
        //playerTransform.rotation = glm::degrees(glm::atan(forward.y, forward.x) - cameraRot);
    }
}
