#pragma once
#include "GameCore.h"
#include <Graphics\texture.h>

// external
#include <json11\json11.hpp>

#include <unordered_map>

struct IGraphicsDevice;
struct StreamReader;
struct StreamWriter;
struct AssetManager;
struct IShaderProgram;
struct IMaterial;
struct Sprite;

ASSET_STRUCT(IShaderProgram)
{
    char shaderPath[MAX_ASSET_PATH];
    u64 shaderConfig;

    std::shared_ptr<IShaderProgram> create(std::shared_ptr<IGraphicsDevice> api, std::shared_ptr<AssetManager> assets);
};

ASSET_STRUCT(ITexture)
{
    char texturePath[MAX_ASSET_PATH];
    TextureSpecification specification;

    std::shared_ptr<ITexture> create(std::shared_ptr<IGraphicsDevice> api, std::shared_ptr<AssetManager> assets);
};

ASSET_STRUCT(IMaterial)
{
    enum class ParameterType : u8
    {
        // texture is currently the only supported parameter type
        TEXTURE,
        MAX
    };

    struct Parameter
    {
        ParameterType type = ParameterType::TEXTURE;
        char value[MAX_ASSET_PATH];
    };

    char shaderAssetPath[MAX_ASSET_PATH];
    std::unordered_map<std::string, Parameter> parameters;

    std::shared_ptr<IMaterial> create(std::shared_ptr<IGraphicsDevice> api, std::shared_ptr<AssetManager> assets);
};

enum class SpriteType
{
    SINGLE, SHEET, STACK,
    MAX
};

ASSET_STRUCT(Sprite)
{
    char materialAssetId[MAX_ASSET_PATH];
    size_t count = 1;
    SpriteType type = SpriteType::SINGLE;
    ivec2 sizeInPixels = ivec2(1);

    std::shared_ptr<Sprite> create(std::shared_ptr<IGraphicsDevice> api, std::shared_ptr<AssetManager> assets);
};

namespace core::asset
{
    // shader asset
    void SerialiseShaderAssetFromJson(
          json11::Json::object const& json_object
        , IShaderProgramAsset& asset);

    void SerialiseShaderAssetToBinary(
          std::shared_ptr<StreamWriter> writer
        , IShaderProgramAsset const& asset);

    void SerialiseShaderAssetFromBinary(
          std::shared_ptr<StreamReader> reader
        , IShaderProgramAsset& asset);

    // texture asset
    void SerialiseTextureAssetFromJson(
         json11::Json::object const& json_object
        , ITextureAsset& asset);

    void SerialiseTextureAssetToBinary(
          std::shared_ptr<StreamWriter> writer
        , ITextureAsset const& asset);

    void SerialiseTextureAssetFromBinary(
          std::shared_ptr<StreamReader> reader
        , ITextureAsset& asset);

    // Material asset
    void SerialiseMaterialAssetFromJson(
          json11::Json::object const& json_object
        , IMaterialAsset& asset);

    void SerialiseMaterialAssetToBinary(
          std::shared_ptr<StreamWriter> writer
        , IMaterialAsset const& asset);

    void SerialiseMaterialAssetFromBinary(
          std::shared_ptr<StreamReader> reader
        , IMaterialAsset& asset);

    // sprite asset
    void SerialiseSpriteAssetFromJson(
          json11::Json::object const& json_object
        , SpriteAsset& asset);

    void SerialiseSpriteAssetToBinary(
          std::shared_ptr<StreamWriter> writer
        , SpriteAsset const& asset);

    void SerialiseSpriteAssetFromBinary(
          std::shared_ptr<StreamReader> reader
        , SpriteAsset& asset);

}