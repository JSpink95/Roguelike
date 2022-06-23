#pragma once
#include "GameCore.h"
#include <Core\Object.h>

#include "Render\SpriteRenderer.h"

/*
    A simple wrapper for the SpriteRenderer.
    If you want to render text, use this
*/

struct TextRenderer : public Object
{
    BIND_OBJECT(TextRenderer);

    static Ref Create(IGraphicsDevice::Ref graphics, SpriteRendererSpecification const& spec);
    TextRenderer(IGraphicsDevice::Ref graphics, SpriteRendererSpecification const& spec);

    void Begin(Entity::Ref camera);
    void Finish();

    void SubmitText(std::string_view text, float x, float y, float z, float size, u32 spriteLayer);

    SpriteRenderer::Ref renderer = nullptr;
};