#include "Render\TextRenderer.h"

TextRenderer::Ref TextRenderer::Create(IGraphicsDevice::Ref graphics, SpriteRendererSpecification const& spec)
{
    return std::make_shared<TextRenderer>(graphics, spec);
}

TextRenderer::TextRenderer(IGraphicsDevice::Ref graphics, SpriteRendererSpecification const& spec)
    : renderer(SpriteRenderer::Create(graphics, spec))
{

}

void TextRenderer::Begin(Entity::Ref camera)
{
    renderer->Begin(camera);
}

void TextRenderer::Finish()
{
    renderer->Finish();
}

void TextRenderer::SubmitText(std::string_view text, float x, float y, float z, float size, u32 spriteLayer)
{
    constexpr std::string_view Glyphs = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

    for (u32 index = 0u; index < text.length(); ++index)
    {
        char character = std::toupper(text.at(index));
        auto spriteIndex = Glyphs.find(character);
        if (spriteIndex != std::string::npos)
        {
            renderer->SubmitSprite(x + size * index, y, z, vec2(size), spriteIndex, spriteLayer);
        }
    }
}