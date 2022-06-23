#pragma once
#include "GameCore.h"

#include <Core\Object.h>

#include <filesystem>
#include <unordered_map>

struct IGraphicsDevice;

#if 0
struct AssetManager
    : public Object
{
    BIND_OBJECT(AssetManager);
    struct IAsset: public Object
    {
        BIND_OBJECT(IAsset)

        virtual ~IAsset() = default;
        virtual void FixupReference(
              std::shared_ptr<IGraphicsDevice> api
            , std::shared_ptr<AssetManager> assets) = 0;
    };

    template<typename T>
    struct TAsset
        : public IAsset
    {
        // omit the BIND_OBJECT macro here, the <T> messes with the name generation

        typename T::AssetType source;
        std::shared_ptr<T> instance;

        TAsset(typename T::AssetType const& as)
            : source(as)
            , instance(nullptr) { }

        void FixupReference(
              std::shared_ptr<IGraphicsDevice> api
            , std::shared_ptr<AssetManager> assets) final
        {
            instance = source.create(api, assets);
        }
    };

    AssetManager(std::shared_ptr<IGraphicsDevice> api)
        : api(api) {}

    std::shared_ptr<IGraphicsDevice> api;
    std::unordered_map<std::string, IAsset::Ref> assets;

    template<typename T, typename TFunc> void ForEachAsset(TFunc func);
    template<typename T> typename T::Ref FindAsset(std::string_view name);

    template<typename T> typename TAsset<T>::Ref EmplaceAsset(
          std::string_view name
        , typename T::AssetType const& new_asset);

};
#include "AssetManager.inl"

namespace core::asset
{
    void ParseDirectoryForFilesWithExtension(
          std::filesystem::path root
        , std::vector<std::filesystem::path>& outPaths
        , std::string_view extension = "");

    void ParseAssetDirectory(
        AssetManager::Ref assets);
}
#endif