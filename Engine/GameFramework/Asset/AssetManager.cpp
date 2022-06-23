#include "AssetManager.h"
#if 0
#include "assets.h"

#include <Graphics\ShaderProgram.h>
#include <Graphics\Material.h>
#include <Platform\FileIO.h>

#include "Game\sprite.h"

#include <filesystem>

namespace core::asset
{
    void ParseDirectoryForFilesWithExtension(
          std::filesystem::path root
        , std::vector<std::filesystem::path>& outPaths
        , std::string_view extension/* = ""*/)
    {
        for (auto& dir : std::filesystem::directory_iterator(root))
        {
            if (dir.is_directory())
            {
                ParseDirectoryForFilesWithExtension(dir, outPaths, extension);
            }
            else if (dir.path().has_extension())
            {
                std::string fileExt = dir.path().extension().generic_string();
                if (fileExt == extension)
                {
                    outPaths.emplace_back(dir.path());
                }
            }
        }
    }

    void ParseAssetDirectory(AssetManager::Ref assets)
    {
        auto assetPath = std::filesystem::path("assets");

        std::vector<std::filesystem::path> binaryAssetPaths;
        ParseDirectoryForFilesWithExtension(assetPath, binaryAssetPaths, ".asset");

        // load binary assets
        for (auto& binaryAssetPath : binaryAssetPaths)
        {
            auto binaryPathString = binaryAssetPath.generic_string();
            if (auto reader = core::OpenStreamReader(binaryPathString))
            {
                AssetHeader header;
                reader->read(&header, sizeof(AssetHeader));

                std::string_view headerName = header.asset_type_name;

                if (headerName == STRINGIFY(IShaderProgramAsset))
                {
                    IShaderProgramAsset asset;
                    SerialiseShaderAssetFromBinary(reader, asset);

                    assets->EmplaceAsset<IShaderProgram>(binaryPathString, asset);
                }
                else if (headerName == STRINGIFY(ITextureAsset))
                {
                    ITextureAsset asset;
                    SerialiseTextureAssetFromBinary(reader, asset);

                    assets->EmplaceAsset<ITexture>(binaryPathString, asset);
                }
                else if (headerName == STRINGIFY(IMaterialAsset))
                {
                    IMaterialAsset asset;
                    SerialiseMaterialAssetFromBinary(reader, asset);

                    assets->EmplaceAsset<IMaterial>(binaryPathString, asset);
                }
                else if (headerName == STRINGIFY(SpriteAsset))
                {
                    SpriteAsset asset;
                    SerialiseSpriteAssetFromBinary(reader, asset);

                    assets->EmplaceAsset<Sprite>(binaryPathString, asset);
                }
                else
                {

                }
                core::CloseStreamReader(reader);
            }
        }
    }
}
#endif