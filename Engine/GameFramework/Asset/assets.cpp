#include "assets.h"
#include "AssetManager.h"

#include <Core\conv.h>
#include <Platform\FileIO.h>

#include "Game\sprite.h"
#include <Graphics\Material.h>
#include <Graphics\ShaderProgram.h>
#include <Graphics\GraphicsAPI.h>

std::shared_ptr<IShaderProgram> IShaderProgramAsset::create(std::shared_ptr<IGraphicsDevice> api, std::shared_ptr<AssetManager> assets)
{
    ShaderProgramConfig config = { shaderConfig };
    return api->CreateShaderProgram(shaderPath, config);
}

std::shared_ptr<ITexture> ITextureAsset::create(std::shared_ptr<IGraphicsDevice> api, std::shared_ptr<AssetManager> assets)
{
    return api->CreateTexture(texturePath, specification);
}

std::shared_ptr<IMaterial> IMaterialAsset::create(std::shared_ptr<IGraphicsDevice> api, std::shared_ptr<AssetManager> assets)
{
    //auto shader = assets->FindAsset<IShaderProgram>(shaderAssetPath);
    //if (shader == nullptr)
    //{
    //    return nullptr;
    //}
    //
    //auto mat = api->CreateMaterial(shader);
    //for (auto& [name, parameter] : parameters)
    //{
    //    switch (parameter.type)
    //    {
    //        case ParameterType::TEXTURE:
    //        {
    //            auto texture = assets->FindAsset<ITexture>(parameter.value);
    //            if (texture == nullptr)
    //            {
    //                texture = ITexture::checkerboard;
    //            }
    //            mat->SetTexture(name, texture);
    //        } break;
    //        default:
    //            // !!
    //            break;
    //    }
    //}
    //
    //return mat;
    return nullptr;
}

std::shared_ptr<Sprite> SpriteAsset::create(std::shared_ptr<IGraphicsDevice> api, std::shared_ptr<AssetManager> assets)
{
    return nullptr;
}

namespace core::asset
{
    // shader asset

    void SerialiseShaderAssetFromJson(
          json11::Json::object const& json_object
        , IShaderProgramAsset& asset)
    {
        strcpy_s(asset.shaderPath, json_object.at("shader_path").string_value().data());
        asset.shaderConfig = json_object.find("shader_config") != json_object.end() ? json_object.at("shader_config").int_value() : 0u;
    }

    void SerialiseShaderAssetToBinary(
          std::shared_ptr<StreamWriter> writer
        , IShaderProgramAsset const& asset)
    {
        writer->write(&asset, sizeof(IShaderProgramAsset));
    }

    void SerialiseShaderAssetFromBinary(
          std::shared_ptr<StreamReader> reader
        , IShaderProgramAsset& asset)
    {
        reader->read(&asset, sizeof(IShaderProgramAsset));
    }

    // texture asset

    void SerialiseTextureAssetFromJson(
          json11::Json::object const& json_object
        , ITextureAsset& asset)
    {
        strcpy_s(asset.texturePath, json_object.at("texture_path").string_value().data());
        asset.specification.filterMode = static_cast<TextureFilter>(json_object.at("filter").int_value());
        asset.specification.wrapMode = static_cast<TextureWrap>(json_object.at("wrap_mode").int_value());
    }

    void SerialiseTextureAssetToBinary(
          std::shared_ptr<StreamWriter> writer
        , ITextureAsset const& asset)
    {
        writer->write(&asset, sizeof(ITextureAsset));
    }

    void SerialiseTextureAssetFromBinary(
        std::shared_ptr<StreamReader> reader
        , ITextureAsset& asset)
    {
        reader->read(&asset, sizeof(ITextureAsset));
    }

    // Material asset

    void SerialiseMaterialAssetFromJson(
          json11::Json::object const& json_object
        , IMaterialAsset& asset)
    {
        strcpy_s(asset.shaderAssetPath, json_object.at("shader_asset_path").string_value().data());

        if (json_object.find("parameters") != json_object.end())
        {
            auto const& parameters_json_object = json_object.at("parameters").object_items();
            for (auto& [key, value] : parameters_json_object)
            {
                auto const& parameter_json_object = value.object_items();

                IMaterialAsset::Parameter parameter;
                parameter.type = (IMaterialAsset::ParameterType)parameter_json_object.at("type").int_value();
                strcpy_s(parameter.value, parameter_json_object.at("value").string_value().data());
                asset.parameters.emplace(key, parameter);
            }
        }
    }

    void SerialiseMaterialAssetToBinary(
          std::shared_ptr<StreamWriter> writer
        , IMaterialAsset const& asset)
    {
        // the shader asset
        writer->write(asset.shaderAssetPath, sizeof(asset.shaderAssetPath));

        // serialise the number of elements specified in the std::unordered_map
        size_t parameter_count = asset.parameters.size();
        writer->write(&parameter_count, sizeof(size_t));

        // now serialise each individual parameter
        for (auto& [parameter_name, parameter_data] : asset.parameters)
        {
            // serialise string length
            size_t parameter_name_length = parameter_name.length();
            writer->write(&parameter_name_length, sizeof(size_t));

            // serialise the string
            writer->write(parameter_name.data(), parameter_name_length);

            // serialise the parameter data
            writer->write(&parameter_data, sizeof(IMaterialAsset::Parameter));
        }
    }

    void SerialiseMaterialAssetFromBinary(
          std::shared_ptr<StreamReader> reader
        , IMaterialAsset& asset)
    {
        reader->read(asset.shaderAssetPath, sizeof(asset.shaderAssetPath));

        size_t parameter_count = 0;
        reader->read(&parameter_count, sizeof(size_t));

        for (size_t parameter_index = 0u; parameter_index < parameter_count; ++parameter_index)
        {
            size_t parameter_name_length = 0;
            reader->read(&parameter_name_length, sizeof(size_t));

            std::string parameter_name(parameter_name_length, '#');
            reader->read(parameter_name.data(), parameter_name_length);

            IMaterialAsset::Parameter parameter_data;
            reader->read(&parameter_data, sizeof(IMaterialAsset::Parameter));

            asset.parameters.emplace(parameter_name, parameter_data);
        }
    }

    // sprite asset

    void SerialiseSpriteAssetFromJson(
          json11::Json::object const& json_object
        , SpriteAsset& asset)
    {
        asset.count = (size_t)json_object.at("sprite_count").int_value();
        asset.type = (SpriteType)json_object.at("type").int_value();
        asset.sizeInPixels = core::string::to_ivec2(json_object.at("size_in_pixels").string_value());
        strcpy_s(asset.materialAssetId, json_object.at("Material_asset_id").string_value().data());
    }

    void SerialiseSpriteAssetToBinary(
        std::shared_ptr<StreamWriter> writer
        , SpriteAsset const& asset)
    {
        writer->write(&asset, sizeof(SpriteAsset));
    }

    void SerialiseSpriteAssetFromBinary(
        std::shared_ptr<StreamReader> reader
        , SpriteAsset& asset)
    {
        reader->read(&asset, sizeof(SpriteAsset));
    }

}