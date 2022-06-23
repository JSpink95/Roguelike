#include "Graphics\OpenGL\Texture_OpenGL.h"
#include "Graphics\OpenGL\OpenGLUtility.h"
#include <stb\stb_image.h>

namespace gl
{
    namespace helper
    {
        GLenum ConvertType(PrimitiveType type)
        {
            switch (type)
            {
                case PrimitiveType::INT8: return GL_BYTE;
                case PrimitiveType::INT16: return GL_SHORT;
                case PrimitiveType::INT32: return GL_INT;
                case PrimitiveType::UINT8: return GL_UNSIGNED_BYTE;
                case PrimitiveType::UINT16: return GL_UNSIGNED_SHORT;
                case PrimitiveType::UINT32: return GL_UNSIGNED_INT;
                case PrimitiveType::FLOAT: return GL_FLOAT;
                default: return GL_NONE;
            }
        }
        GLenum GetFormatFromComponentCount(int components)
        {
            switch (components)
            {
                case 1: return GL_RED;
                case 2: return GL_RG;
                case 3: return GL_RGB;
                case 4: return GL_RGBA;
                default: return GL_NONE;
            }
        }

        GLenum GetInternalFormatFromComponentCount(int components, GLenum type)
        {
            switch (components)
            {
                case 1: return type == GL_FLOAT ? GL_R32F : GL_RED;
                case 2: return type == GL_FLOAT ? GL_RG32F : GL_RG;
                case 3: return type == GL_FLOAT ? GL_RGB32F : GL_RGB;
                case 4: return type == GL_FLOAT ? GL_RGBA32F : GL_RGBA;
                default: return GL_NONE;
            }
        }
    }

    Texture::Ref Texture::Create(
          u32 width
        , u32 height
        , u32 components
        , PrimitiveType type
        , void* pixels
        , TextureSpecification const& specification)
    {
        auto result = std::make_shared<Texture>();

        if (!result->InitialiseTexture(width, height, components, type, pixels, specification))
        {
            return nullptr;
        }

        return result;
    }

    bool Texture::InitialiseTexture(
          u32 width
        , u32 height
        , u32 components
        , PrimitiveType type
        , void* pixels
        , TextureSpecification const& specification)
    {
        glGenTextures(1, &id);
        glBindTexture(GL_TEXTURE_2D, id);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, gl::FromFilter(specification.filterMode));
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, gl::FromFilter(specification.filterMode));

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, gl::FromWrap(specification.wrapMode));
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, gl::FromWrap(specification.wrapMode));

        glTexImage2D(
            GL_TEXTURE_2D
            , 0
            , helper::GetInternalFormatFromComponentCount(components, helper::ConvertType(type))
            , width
            , height
            , 0
            , helper::GetFormatFromComponentCount(components)
            , helper::ConvertType(type)
            , pixels);

        glBindTexture(GL_TEXTURE_2D, 0);
        return true;
    }

    std::shared_ptr<Texture> CreateTextureFromPixels(
          int width
        , int height
        , int components
        , PrimitiveType type
        , void* pixels
        , TextureSpecification const& specification/*= {}*/)
    {
        return Texture::Create(width, height, components, type, pixels, specification);
    }

    std::shared_ptr<Texture> CreateTextureFromPath(
          std::shared_ptr<GraphicsAPI> api
        , std::string_view path
        , TextureSpecification const& specification)
    {
        int width, height, components;
        stbi_uc* pixels = stbi_load(path.data(), &width, &height, &components, 0);

        auto result = CreateTextureFromPixels(
              width
            , height
            , components
            , PrimitiveType::UINT8
            , pixels
            , specification);

        stbi_image_free(pixels);

        return result;
    }
}