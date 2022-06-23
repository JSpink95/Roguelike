#pragma once
#include "GameCore.h"
#include "Game\System.h"

struct Scene;

struct ScriptSystem : public ISystem
{
    BIND_OBJECT(ScriptSystem);

    enum class State
    {
        BEGIN, UPDATE, FINISH,
        MAX
    };

    std::shared_ptr<Scene> world = nullptr;
    State activeState = State::BEGIN;

    ScriptSystem(std::shared_ptr<Scene> owner)
        : world(owner) {}

    inline void SetState(State newState)
    {
        activeState = newState;
    }

    void Update(float delta) final;
};