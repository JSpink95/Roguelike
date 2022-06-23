#pragma once
#include "GameCore.h"
#include "Game\Sprite.h"
#include "Game\Entity.h"

#include <Core\Object.h>
#include <Graphics\GraphicsAPI.h>


#include "Render\QuadBatchRenderer.h"

#include <vector>

struct SpriteRendererSpecification
{
    bool flipTextureCoords = false;
    ivec2 textureSize = ivec2(1);
    ivec2 spriteSize = ivec2(1);
    std::vector<TextureObject> textures = {};
};

struct SpriteRenderer : public Object
{
    BIND_OBJECT(SpriteRenderer);

    struct Vertex
    {
        static inline VertexAttributeTypeList Attributes =
        {
            VertexAttributeType::FLOAT3,    // Position(Vec3)
            VertexAttributeType::FLOAT2,    // Texcoord(Vec2)
            VertexAttributeType::FLOAT2     // TextureID,Alpha(Vec2)
        };

        vec3 a_Position;
        vec2 a_Texcoord;
        float a_TexID, a_Alpha;
    };

    static Ref Create(IGraphicsDevice::Ref api, SpriteRendererSpecification const& spec);

    SpriteRenderer(IGraphicsDevice::Ref api, SpriteRendererSpecification const& spec);

    void SetWorldTransform(glm::mat4 const& transform);

    void Begin(Entity::Ref camera);
    void Finish();

    void SubmitSprite(float x, float y, float z, float size, Sprite::Ref sprite, float alpha = 1.0f);
    void SubmitSprite(float x, float y, float z, vec2 const& size, Sprite::Ref sprite, float alpha = 1.0f);
    void SubmitSprite(float x, float y, float z, vec2 const& size, u32 spriteIndex, u32 spriteLayer, float alpha = 1.0f);

    Sprite::Ref CreateSprite(u32 layer, u32 index) const;
    SpriteSheet::Ref GetSpriteSheet(u32 layer) const;

    IGraphicsDevice::Ref graphicsDevice = nullptr;
    SpriteRendererSpecification specification = {};

    IPipeline::Ref pipeline = nullptr;
    std::vector<SpriteSheet::Ref> spriteSheets = {};
    VertexBatchRenderer<Vertex>::Ref batcher = nullptr;
};
