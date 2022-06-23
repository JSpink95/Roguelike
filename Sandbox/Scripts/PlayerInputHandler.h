#pragma once
#include <Core.h>
#include <Core\Object.h>
#include <Core\KeyboardInputManager.h>

struct InputCommand : public Object
{
    BIND_OBJECT(InputCommand);

    virtual void OnInputBegin() = 0;
    virtual void OnInputEnd() = 0;
};

struct Generic_InputCommand : public InputCommand
{
    BIND_OBJECT(Generic_InputCommand);

    static Ref Create()
    {
        return std::make_shared<Generic_InputCommand>();
    }

    void OnInputBegin() final { inputDown = true; }
    void OnInputEnd() final { inputDown = false; }

    bool inputDown = false;
};

struct PlayerInputHandler : public Object
{
    BIND_OBJECT(PlayerInputHandler);

    static Ref Create(KeyboardInputManager::Ref keyboard)
    {
        return std::make_shared<PlayerInputHandler>(keyboard);
    }

    PlayerInputHandler(KeyboardInputManager::Ref keyboard);

    void BindLeftMovementInput(SDL_Keycode keycode);
    void BindRightMovementInput(SDL_Keycode keycode);
    void BindJumpMovementInput(SDL_Keycode keycode);

    KeyboardInputManager::Ref keyboard = nullptr;
    Generic_InputCommand::Ref moveLeftCommand = nullptr;
    Generic_InputCommand::Ref moveRightCommand = nullptr;
    Generic_InputCommand::Ref jumpCommand = nullptr;
};