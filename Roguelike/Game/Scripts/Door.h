#pragma once
#include <GameCore.h>
#include <Game\script_component.h>

struct DoorScript
    : public IScriptBehaviour
{
    BIND_OBJECT(DoorScript)
    std::shared_ptr<Entity> player = nullptr;

    bool isOpen = false;
    bool isLocked = false;

    float targetRotation = 0.0f;
    float rotation = 0.0f;
    float doorOpenSpeed = 20.0f;

    // iscript_behaviour
    void OnBegin() final;
    void OnUpdate(float dt) final;
    void OnFinish() final {}

    float GetDistanceToPlayer() const;
    void UpdateDoorOpenAnimation(float dt);
};