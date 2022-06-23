#include "Game\Sprite.h"

vec2 SpriteSheetSpecification::GetSpriteSizeUV() const
{
    float sizeU = (1.0f / spriteSheetSizePixels.x) * spriteSizePixels.x;
    float sizeV = (1.0f / spriteSheetSizePixels.y) * spriteSizePixels.y;
    return vec2(sizeU, sizeV);
}

u32 SpriteSheetSpecification::GetColumnCount() const
{
    return spriteSheetSizePixels.x / spriteSizePixels.x;
}
u32 SpriteSheetSpecification::GetRowCount() const
{
    return spriteSheetSizePixels.y / spriteSizePixels.y;
}

SpriteSheet::Ref SpriteSheet::Create(SpriteSheetSpecification const& spec)
{
    return std::make_shared<SpriteSheet>(spec);
}

SpriteSheet::SpriteSheet(SpriteSheetSpecification const& spec)
    : specification(spec)
{
    vec2 spriteSize = specification.GetSpriteSizeUV();

    for (u32 spriteIndex = 0u; spriteIndex < spec.GetColumnCount() * spec.GetRowCount(); ++spriteIndex)
    {
        u32 x = spriteIndex % specification.GetColumnCount();
        u32 y = spriteIndex / specification.GetColumnCount();

        vec4& spriteUVBounds = cachedUVBounds.emplace_back();
        spriteUVBounds.x = spriteSize.x * (float)x;
        spriteUVBounds.y = spriteSize.y * (float)y;
        spriteUVBounds.z = spriteUVBounds.x + spriteSize.x;
        spriteUVBounds.w = spriteUVBounds.y + spriteSize.y;
    }
}

vec4 SpriteSheet::GetUVBounds(u32 spriteIndex) const
{
    if (spriteIndex >= cachedUVBounds.size())
        return vec4(0.0f);

    return cachedUVBounds.at(spriteIndex);
}

Sprite::Ref Sprite::Create(SpriteSheet::Ref sheet, u32 index)
{
    return std::make_shared<Sprite>(sheet, index);
}

Sprite::Sprite(SpriteSheet::Ref sheet, u32 index)
    : spriteSheet(sheet)
    , spriteIndex(index)
{

}
