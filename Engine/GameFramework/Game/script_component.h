#pragma once
#include "GameCore.h"
#include "Game\components.h"

struct IScriptBehaviour : public Object
{
    BIND_OBJECT(IScriptBehaviour)
public:
    std::shared_ptr<Entity> owner;

    virtual ~IScriptBehaviour() = default;
    inline auto GetOwner() { return owner; }

    virtual void OnBegin() {}
    virtual void OnFinish() {}
    virtual void OnUpdate(float dt) {}
};

struct ScriptComponent : public IComponent
{
    BIND_OBJECT(ScriptComponent)
public:
    std::unordered_map<u64, std::shared_ptr<IScriptBehaviour>> bindings;

    template<typename T> std::shared_ptr<T> Find();
    template<typename T, typename...TArgs> std::shared_ptr<T> Add(TArgs&&...args);
    template<typename T> void Remove();

    void Invoke_OnBegin();
    void Invoke_OnFinish();
    void Invoke_OnUpdate(float delta_time);
};

#include "Game\script_component.inl"