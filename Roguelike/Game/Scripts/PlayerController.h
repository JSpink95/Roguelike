#pragma once
#include <Core.h>
#include <Core\KeyboardInputManager.h>
#include <Game\script_component.h>
#include <Game\sprite.h>

struct AppWindow;
struct Entity;

struct PlayerController
    : public IScriptBehaviour
{
    BIND_OBJECT(PlayerController)
    static constexpr WORD INPUT_KEY_STRAFE_LEFT = VK_LEFT;
    static constexpr WORD INPUT_KEY_STRAFE_RIGHT = VK_RIGHT;
    static constexpr WORD INPUT_KEY_FORWARD = VK_UP;
    static constexpr WORD INPUT_KEY_BACKWARD = VK_DOWN;
    static constexpr float MAX_MOVE_SPEED = 100.0f;

    std::shared_ptr<AppWindow> window;
    std::shared_ptr<Entity> cameraEntity;

    struct InputState;
    std::unique_ptr<InputState> input;

    PlayerController(std::shared_ptr<AppWindow> w);
    ~PlayerController();

    void OnKeyDown(WORD key, bool repeat);
    void OnKeyUp(WORD key);

    void OnBegin() final;
    void OnFinish() final;
    void OnUpdate(float delta) final;
};