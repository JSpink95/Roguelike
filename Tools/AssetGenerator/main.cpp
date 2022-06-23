#include <Core.h>
#include <Core\conv.h>
#include <Asset\assets.h>

#include <Graphics\OpenGL\CoreGL.h>

#include <json11\json11.hpp>
#include <stb\stb_image.h>

#include <string>
#include <fstream>
#include <filesystem>

void process_asset(std::string_view path)
{
    int width, height, components;
    auto pixels = stbi_load(path.data(), &width, &height, &components, 0);

    // get texture name
    std::string path_str = path.data();
    size_t start_index = path_str.find_last_of('/') + 1;
    if (start_index == std::string::npos) start_index = 0;

    std::string asset_name = path_str.substr(start_index, path_str.find_last_of('.') - start_index);

    std::string texture_binary_path = "assets/bin/" + asset_name + "_texture.asset";
    std::string Material_binary_path = "assets/bin/" + asset_name + "_Material.asset";

    ivec2 sprite_size = ivec2(height, height); // assuming that sprites are square and laid out horizontally
    int sprite_count = width / height;

    {
        // generate texture json
        {
            json11::Json texture_json = json11::Json::object{
                { "asset_type", "ITextureAsset" },
                { "texture_path", path_str },
                { "filter", static_cast<int>(TextureFilter::POINT) },
                { "wrap_mode", static_cast<int>(TextureWrap::CLAMP) },
            };

            std::string texture_json_string = texture_json.dump();
            std::ofstream output("assets/textures/" + asset_name + "_texture.json");
            output.write(texture_json_string.data(), texture_json_string.length());
        }

        // generate Material json
        {
            // TODO(jack): will need to change this
            json11::Json Material_json = json11::Json::object{
                { "asset_type", "IMaterialAsset" },
                { "shader_asset_path", "assets/bin/sprite_shader.asset" },
                { "parameters", json11::Json::object {
                    { "_Texture", json11::Json::object { 
                        { "type", (int)IMaterialAsset::ParameterType::TEXTURE },
                        { "value", texture_binary_path },
                    } },
                } }
            };

            std::string Material_json_string = Material_json.dump();
            std::ofstream output("assets/Materials/" + asset_name + "_Material.json");
            output.write(Material_json_string.data(), Material_json_string.length());
        }

        // generate sprite json
        {
            json11::Json sprite_json = json11::Json::object{
                { "asset_type", "sprite_asset" },
                { "Material_asset_id", Material_binary_path },
                { "type", (int)SpriteType::STACK },
                { "sprite_count", sprite_count },
                { "size_in_pixels", core::string::convert<ivec2>(sprite_size)}
            };

            std::string sprite_json_string = sprite_json.dump();
            std::ofstream output("assets/sprites/" + asset_name + "_sprite.json");
            output.write(sprite_json_string.data(), sprite_json_string.length());
        }
    }

    stbi_image_free(pixels);
}

void process_path(std::string_view input)
{
    for (auto& path : std::filesystem::directory_iterator(input))
    {
        auto path_string = path.path().generic_string();
        if (path.is_directory())
        {
            process_path(path_string);
        }
        else if (path.path().has_extension() && path.path().extension() == ".png")
        {
            process_asset(path_string);
        }
    }
}

int main(int argc, char* argv[])
{
    for (int arg_index = 1; arg_index < argc; ++arg_index)
    {
        std::string_view path = argv[arg_index];
        process_path(path);
    }

    return 0;
}