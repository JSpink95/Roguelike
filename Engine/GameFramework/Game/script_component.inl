template<typename T> std::shared_ptr<T> ScriptComponent::Find()
{
    auto it = bindings.find(T::ThisTypeID);
    if (it != bindings.end())
    {
        return std::dynamic_pointer_cast<T>(it->second);
    }

    return nullptr;
}
template<typename T, typename...TArgs> inline std::shared_ptr<T> ScriptComponent::Add(TArgs&&...args)
{
    auto script = std::make_shared<T>(std::forward<TArgs>(args)...);
    script->owner = owner;

    bindings.emplace(T::ThisTypeID, script);
    return script;
}
template<typename T> inline void ScriptComponent::Remove()
{
    bindings.erase(T::ThisTypeID);
}