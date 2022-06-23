#pragma once
#include "GameCore.h"

#include <Graphics\GraphicsAPI.h>

struct SpriteSheetSpecification
{
    ivec2 spriteSheetSizePixels = ivec2(1u);
    ivec2 spriteSizePixels = ivec2(1u);
    ITexture::Ref texture = nullptr;

    vec2 GetSpriteSizeUV() const;
    u32 GetColumnCount() const;
    u32 GetRowCount() const;
};

struct SpriteSheet : public Object
{
    BIND_OBJECT(SpriteSheet);

    static Ref Create(SpriteSheetSpecification const& spec);

    SpriteSheet(SpriteSheetSpecification const& spec);
    vec4 GetUVBounds(u32 spriteIndex) const;

    SpriteSheetSpecification specification = {};
    std::vector<vec4> cachedUVBounds = {};
};

struct Sprite : public Object
{
    BIND_OBJECT(Sprite);

    static Ref Create(SpriteSheet::Ref spriteSheet, u32 spriteIndex);
    Sprite(SpriteSheet::Ref sheet, u32 index);

    SpriteSheet::Ref spriteSheet = nullptr;
    u32 spriteIndex = 0u;
};