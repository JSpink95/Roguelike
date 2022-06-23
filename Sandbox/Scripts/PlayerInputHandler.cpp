#include "Scripts\PlayerInputHandler.h"

PlayerInputHandler::PlayerInputHandler(KeyboardInputManager::Ref keyboard)
    : keyboard(keyboard)
{
    moveLeftCommand = Generic_InputCommand::Create();
    moveRightCommand = Generic_InputCommand::Create();
    jumpCommand = Generic_InputCommand::Create();
}

void PlayerInputHandler::BindLeftMovementInput(SDL_Keycode keycode)
{
    auto onKeyDown = [this](void* context, SDL_Keycode key, bool repeat) -> void
    {
        moveLeftCommand->OnInputBegin();
    };

    auto onKeyRelease = [this](void* context, SDL_Keycode key) -> void
    {
        moveLeftCommand->OnInputEnd();
    };

    // TODO(jack): remove binding to previous key
    keyboard->AddKeyEventHandler(keycode, this, onKeyDown, onKeyRelease);
}

void PlayerInputHandler::BindRightMovementInput(SDL_Keycode keycode)
{
    auto onKeyDown = [this](void* context, SDL_Keycode key, bool repeat) -> void
    {
        moveRightCommand->OnInputBegin();
    };

    auto onKeyRelease = [this](void* context, SDL_Keycode key) -> void
    {
        moveRightCommand->OnInputEnd();
    };

    // TODO(jack): remove binding to previous key
    keyboard->AddKeyEventHandler(keycode, this, onKeyDown, onKeyRelease);
}

void PlayerInputHandler::BindJumpMovementInput(SDL_Keycode keycode)
{
    auto onKeyDown = [this](void* context, SDL_Keycode key, bool repeat) -> void
    {
        jumpCommand->OnInputBegin();
    };

    auto onKeyRelease = [this](void* context, SDL_Keycode key) -> void
    {
        jumpCommand->OnInputEnd();
    };

    // TODO(jack): remove binding to previous key
    keyboard->AddKeyEventHandler(keycode, this, onKeyDown, onKeyRelease);
}