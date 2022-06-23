#include "Game\script_component.h"

void ScriptComponent::Invoke_OnBegin()
{
    for (auto& [id, script] : bindings)
        script->OnBegin();
}

void ScriptComponent::Invoke_OnFinish()
{
    for (auto& [id, script] : bindings)
        script->OnFinish();
}

void ScriptComponent::Invoke_OnUpdate(float deltaTime)
{
    for (auto& [id, script] : bindings)
        script->OnUpdate(deltaTime);
}
