#include "Graphics\OpenGL\Material_OpenGL.h"
#include "Graphics\OpenGL\ShaderProgram_OpenGL.h"
#include "Graphics\OpenGL\CoreGL.h"

namespace gl
{
    constexpr GLenum shader_properties[2] = { GL_TYPE, GL_NAME_LENGTH };

    Material::Material(std::shared_ptr<IShaderProgram> shader)
        : IMaterial(shader)
    {
        auto glshader = std::dynamic_pointer_cast<ShaderProgram>(shader);

        GLint uniform_count = 0;
        glGetProgramInterfaceiv(glshader->id, GL_UNIFORM, GL_ACTIVE_RESOURCES, &uniform_count);

        for (GLint uniform = 0; uniform < uniform_count; ++uniform)
        {
            GLint values[5];
            glGetProgramResourceiv(glshader->id, GL_UNIFORM, uniform, 2, shader_properties, 2, NULL, values);

            // only care about 2D textures _so far_
            if (values[0] == GL_SAMPLER_2D)
            {
                std::vector<GLchar> name_data(values[1], '\0');
                glGetProgramResourceName(glshader->id, GL_UNIFORM, uniform, name_data.size(), NULL, &name_data[0]);

                std::string parameter_name = name_data.data();
                parameters.emplace(parameter_name, std::make_shared<MaterialParameter_Texture>());
            }

        }
    }
    
    void Material::Bind()
    {
        shader->Begin();

        for (auto [name, parameter] : parameters)
        {
            parameter->upload_to_shader(shader);
        }
    }

    void Material::SetTexture(std::string_view name, std::shared_ptr<ITexture> tex)
    {
        std::string id = std::string(name);
        if (parameters.find(id) != parameters.end())
        {
            auto param = parameters.at(id);
            if (param->GetTypeID() == MaterialParameter_Texture::ThisTypeID)
            {
                auto param_block = std::dynamic_pointer_cast<MaterialParameter_Texture>(param);
                param_block->texture = tex;
            }
        }
    }
}