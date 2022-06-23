#include <iostream>
#include <fstream>
#include <string>
#include <filesystem>
#include <Windows.h>

#include <Core.h>
#include <GameCore.h>
#include <Asset\assets.h>
#include <Platform\FileIO.h>

#include <json11\json11.hpp>

// for mbstowcs
#pragma warning (disable : 4996)

enum class ErrorCode
{
    ok = 0,
    json_parse,
    invalid_asset_format,
    missing_type,
    invalid_asset_type,
    failed_to_open_stream,
}; 

void parse_json(std::string_view json_path)
{
    std::ifstream asset_file(json_path.data());
    std::vector<char> file_buffer((std::istreambuf_iterator<char>(asset_file)), std::istreambuf_iterator<char>());

    std::string binary_path(json_path.data());
    size_t start_index = binary_path.find_last_of('/') + 1;
    if (start_index == std::string::npos) start_index = 0;
    binary_path = "assets/bin/" + binary_path.substr(start_index, binary_path.find_last_of('.') - start_index) + ".asset";

    std::string json_string(file_buffer.begin(), file_buffer.end());
    
    std::string error = "";
    auto json = json11::Json::parse(json_string, error);

    auto const& json_object = json.object_items();
    std::string_view asset_type = json_object.at("asset_type").string_value();

    AssetHeader header;
    strcpy_s(header.asset_type_name, asset_type.data());

    if (asset_type == STRINGIFY(IShaderProgramAsset))
    {
        header.asset_data_size = sizeof(IShaderProgramAsset);

        IShaderProgramAsset asset;
        core::asset::SerialiseShaderAssetFromJson(json_object, asset);

        if (auto writer = core::OpenStreamWriter(binary_path))
        {
            writer->write(&header, sizeof(AssetHeader));
            core::asset::SerialiseShaderAssetToBinary(writer, asset);
            core::CloseStreamWriter(writer);
        }
    }
    else if (asset_type == STRINGIFY(ITextureAsset))
    {
        header.asset_data_size = sizeof(ITextureAsset);

        ITextureAsset asset;
        core::asset::SerialiseTextureAssetFromJson(json_object, asset);

        if (auto writer = core::OpenStreamWriter(binary_path))
        {
            writer->write(&header, sizeof(AssetHeader));
            core::asset::SerialiseTextureAssetToBinary(writer, asset);
            core::CloseStreamWriter(writer);
        }
    }
    else if (asset_type == STRINGIFY(IMaterialAsset))
    {
        header.asset_data_size = sizeof(IMaterialAsset);

        IMaterialAsset asset;
        core::asset::SerialiseMaterialAssetFromJson(json_object, asset);

        if (auto writer = core::OpenStreamWriter(binary_path))
        {
            writer->write(&header, sizeof(AssetHeader));
            core::asset::SerialiseMaterialAssetToBinary(writer, asset);
            core::CloseStreamWriter(writer);
        }
    }
    else if(asset_type == STRINGIFY(SpriteAsset))
    {
        header.asset_data_size = sizeof(SpriteAsset);

        SpriteAsset asset;
        core::asset::SerialiseSpriteAssetFromJson(json_object, asset);

        if (auto writer = core::OpenStreamWriter(binary_path))
        {
            writer->write(&header, sizeof(AssetHeader));
            core::asset::SerialiseSpriteAssetToBinary(writer, asset);
            core::CloseStreamWriter(writer);
        }
    }
    else
    {
        // :/
    }
}

void parse_input(std::string_view input)
{
    LOG("\tProcessing (\"%s\")...", input.data());

    for (auto directory : std::filesystem::directory_iterator(input))
    {
        if (directory.is_directory())
        {
            auto directory_string = directory.path().generic_string();
            parse_input(directory_string);
        }
        else if (directory.path().has_extension() && directory.path().extension() == ".json")
        {
            auto json_path = directory.path().generic_string();
            parse_json(json_path);
        }
    }
}

int main(int argc, char* argv[])
{
    if (argc == 1)
    {
        LOG_ERROR("No assets provided!");
        return -1;
    }

    LOG("Preparing to process assets...");
    for (int arg_index = 1; arg_index < argc; ++arg_index)
    {
        std::string_view input(argv[arg_index]);
        parse_input(input);
    }
    LOG("Finished processing assets!");

    return 0;
}