#pragma once
#include <Core.h>
#include <Core\KeyboardInputManager.h>
#include <Game\script_component.h>

struct Entity;
struct AppWindow;

struct CameraController
    : public IScriptBehaviour
{
    BIND_OBJECT(CameraController)
    static constexpr WORD INPUT_KEY_ROT_CAMERA_LEFT = 'A';
    static constexpr WORD INPUT_KEY_ROT_CAMERA_RIGHT = 'D';
    static constexpr WORD INPUT_KEY_TILT_CAMERA_UP = 'W';
    static constexpr WORD INPUT_KEY_TILT_CAMERA_DOWN = 'S';

    std::shared_ptr<AppWindow> window;
    std::shared_ptr<Entity> playerEntity;

    struct InputState;
    std::unique_ptr<InputState> input;

    float cameraTargetRotation = 45.0f;
    float cameraTilt = 10.0f;

    CameraController(std::shared_ptr<AppWindow> w);
    ~CameraController();

    void OnKeyDown(WORD key, bool repeat);
    void OnKeyUp(WORD key);

    void OnBegin() final;
    void OnFinish() final;
    void OnUpdate(float delta) final;
};