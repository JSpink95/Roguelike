#pragma once
#include "Core.h"
#include "Graphics\types.h"
#include "Graphics\texture.h"
#include "Graphics\OpenGL\CoreGL.h"

namespace gl
{
    struct GraphicsAPI;

    struct Texture : public ITexture
    {
        BIND_OBJECT(Texture);

        static Ref Create(
              u32 width
            , u32 height
            , u32 components
            , PrimitiveType type
            , void* pixels
            , TextureSpecification const& specification);

        GLuint id;

    private:
        bool InitialiseTexture(
              u32 width
            , u32 height
            , u32 components
            , PrimitiveType type
            , void* pixels
            , TextureSpecification const& specification);
    };

    Texture::Ref CreateTextureFromPixels(
          int width
        , int height
        , int components
        , PrimitiveType type
        , void* pixels
        , TextureSpecification const& specification);

    Texture::Ref CreateTextureFromPath(
          std::shared_ptr<GraphicsAPI> api
        , std::string_view path
        , TextureSpecification const& specification);
}
