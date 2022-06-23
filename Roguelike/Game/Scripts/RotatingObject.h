#pragma once
#include <GameCore.h>
#include <Game\script_component.h>

struct RotatingObject
    : public IScriptBehaviour
{
    BIND_OBJECT(RotatingObject)
    float rotationSpeed = 1.0f;

    RotatingObject(float rotSpeed)
        : rotationSpeed(rotSpeed) { }

    void OnBegin() final {}
    void OnFinish() final {}
    void OnUpdate(float deltaTime) final;
};