#include "Graphics\Material.h"
#include "Graphics\texture.h"

void MaterialParameter_Texture::upload_to_shader(std::shared_ptr<IShaderProgram> shader)
{
    shader->UploadTexture(texture, binding);
}

IMaterial::IMaterial(std::shared_ptr<IShaderProgram> shader)
    : shader(shader)
{
}
