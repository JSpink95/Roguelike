#include "Game\Scripts\CameraController.h"

#include "Core\window.h"

#include "Game\entity.h"
#include "Game\components.h"

struct CameraController::InputState
{
    bool cameraTiltUpPressed : 1;
    bool cameraTiltDownPressed : 1;

    InputState()
        : cameraTiltUpPressed(false)
        , cameraTiltDownPressed(false) {}
};

CameraController::CameraController(std::shared_ptr<AppWindow> w)
    : input(std::make_unique<InputState>())
    , window(w)
{

}

CameraController::~CameraController()
{
    input.reset();
}

void CameraController::OnKeyDown(WORD key, bool repeat)
{
    switch (key)
    {
        case INPUT_KEY_ROT_CAMERA_LEFT:
        {
            if (!repeat)
            {
                cameraTargetRotation += 45.0f;
            }
        } break;

        case INPUT_KEY_ROT_CAMERA_RIGHT:
        {
            if (!repeat)
            {
                cameraTargetRotation -= 45.0f;
            }
        } break;

        case INPUT_KEY_TILT_CAMERA_UP:
        {
            input->cameraTiltUpPressed = true;
        } break;

        case INPUT_KEY_TILT_CAMERA_DOWN:
        {
            input->cameraTiltDownPressed = true;
        } break;
    }
}

void CameraController::OnKeyUp(WORD key)
{
    switch (key)
    {
        case INPUT_KEY_TILT_CAMERA_UP:
        {
            input->cameraTiltUpPressed = false;
        } break;

        case INPUT_KEY_TILT_CAMERA_DOWN:
        {
            input->cameraTiltDownPressed = false;
        } break;
    }
}

void CameraController::OnBegin()
{
    playerEntity = GetOwner()->world->FindEntityByTag("player");
    //window->keyboard->RegisterKeyListener(this);
}

void CameraController::OnFinish()
{
    //window->keyboard->UnregisterKeyListener(this);
}

void CameraController::OnUpdate(float delta)
{
    if (input->cameraTiltUpPressed)
    {
        cameraTilt += 6.0f * delta;
    }

    if (input->cameraTiltDownPressed)
    {
        cameraTilt -= 6.0f * delta;
    }

    cameraTilt = glm::max(1.0f, cameraTilt);

    auto& camera_transform = GetOwner()->GetComponent<TransformComponent>();
    auto& player_transform = playerEntity->GetComponent<TransformComponent>();

    camera_transform.position = glm::mix(camera_transform.position, player_transform.position, delta * 8.0f);
    //camera_transform.rotation = glm::mix(camera_transform.rotation, cameraTargetRotation, delta * 6.0f);
}