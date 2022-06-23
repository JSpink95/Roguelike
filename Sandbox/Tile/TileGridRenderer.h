#pragma once
#include <Core.h>
#include <Core\Object.h>

#include <Graphics\GraphicsAPI.h>

#include <vector>

struct TileGridRendererSpecification
{
    ivec2 imageSizePixels = ivec2(1);
    ivec2 spriteSizePixels = ivec2(1);
    std::vector<TextureObject> textures;
};

struct TileGridRenderer : public Object
{
    BIND_OBJECT(TileGridRenderer);

    struct InternalData;
    struct Vertex
    {
        static inline VertexAttributeTypeList Attributes =
        {
            VertexAttributeType::FLOAT3,    // Position(Vec3)
            VertexAttributeType::FLOAT2,    // Texcoord(Vec2)
            VertexAttributeType::FLOAT      // TextureID(Int)
        };

        vec3 a_Position;
        vec2 a_Texcoord;
        float a_TexID;
    };

    static Ref Create(IGraphicsDevice::Ref api, TileGridRendererSpecification const& spec);

    TileGridRenderer(IGraphicsDevice::Ref api, TileGridRendererSpecification const& spec);

    void OpenVertexStream();
    void CloseVertexStream();

    void SubmitTile(float x, float y, int spriteIndex, int textureIndex);
    void SubmitTile(float x, float y, float size, int spriteIndex, int textureIndex);
    void SubmitVertexStream(Vertex* vertices, u64 count);

    IGraphicsDevice::Ref graphicsApi;
    TileGridRendererSpecification specification;
    std::unique_ptr<InternalData> internalData;
};
