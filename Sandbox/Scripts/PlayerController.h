#pragma once
#include <Core.h>
#include <Core\KeyboardInputManager.h>
#include <Game\script_component.h>

#include "Scripts\PlayerInputHandler.h"

struct PlayerController : public IScriptBehaviour
{
    BIND_OBJECT(PlayerController);

    PlayerController(KeyboardInputManager::Ref keyboard);

    void OnBegin() final;
    void OnUpdate(float dt) final;
    void OnFinish() final {}

    PlayerInputHandler::Ref inputHandler = nullptr;
};
