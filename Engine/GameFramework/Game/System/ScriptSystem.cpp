#include "Game\System\ScriptSystem.h"
#include "Game\Scene.h"
#include "Game\script_component.h"

void ScriptSystem::Update(float delta)
{
    auto scripts = world->registry.view<ScriptComponent>();
    switch (activeState)
    {
        case State::BEGIN:
            for (auto [id, sc] : scripts.each())
            {
                sc.Invoke_OnBegin();
            }
            break;
        case State::UPDATE:
            for (auto [id, sc] : scripts.each())
            {
                sc.Invoke_OnUpdate(delta);
            }
            break;
        case State::FINISH:
            for (auto [id, sc] : scripts.each())
            {
                sc.Invoke_OnFinish();
            }
            break;
        default: // nop
            LOG_WARNING("script_system::update invalid state(%i)!", (int)activeState);
            break;

    }
}