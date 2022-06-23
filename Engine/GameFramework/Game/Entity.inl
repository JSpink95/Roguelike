template<typename T, typename...TArgs> inline T& Entity::AddComponent(TArgs&&...args)
{
    auto& component = world->registry.emplace<T>(id, std::forward<TArgs>(args)...);
    component.owner = GetEntityRef();

    return component;
}
template<typename T> inline void Entity::RemoveComponent()
{
    world->registry.erase<T>(id);
}
template<typename T> inline T& Entity::GetComponent()
{
    return world->registry.get<T>(id);
}
template<typename T> inline bool Entity::HasComponent()
{
    return world->registry.any_of<T>(id);
}