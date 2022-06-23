template<typename T, typename TFunc> void AssetManager::ForEachAsset(TFunc func)
{
    for (auto& [key, value] : assets)
    {
        if (auto asset = std::dynamic_pointer_cast<TAsset<T>>(value))
        {
            func(key, asset->instance);
        }
    }
}
template<typename T> typename T::Ref AssetManager::FindAsset(std::string_view name)
{
    std::string id = name.data();

    auto it = assets.find(id);
    if (it == assets.end())
    {
        return nullptr;
    }

    if (it->second->GetTypeID() != core::TypeID<T>::get())
    {
        // found an asset, but it isn't the correct type
        return nullptr;
    }

    auto asset = std::dynamic_pointer_cast<TAsset<T>>(it->second);

    // this check here essentially only loads in the direct assets when we actually need them, so say that we don't use a shader it will never be created etc.
    if (asset->instance == nullptr)
    {
        asset->FixupReference(api, GetAssetManagerRef());
    }

    return asset->instance;
}
template<typename T> typename AssetManager::TAsset<T>::Ref AssetManager::EmplaceAsset(
      std::string_view name
    , typename T::AssetType const& new_asset)
{
    std::string id = name.data();

    auto it = assets.find(id);
    if (it != assets.end())
    {
        return std::dynamic_pointer_cast<TAsset<T>>(it->second);
    }

    auto asset = std::make_shared<TAsset<T>>(new_asset);
    assets.emplace(id, asset);

    return asset;
}