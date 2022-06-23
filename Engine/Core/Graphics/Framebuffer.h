#pragma once
#include "Core.h"
#include "Core\Object.h"
#include "Graphics\Texture.h"

#include <vector>
#include <optional>

struct IRenderPass;

#if 0
enum class ColorFormat : u8
{
    RED,
    RG,
    RGB,
    RGBA,
    RGBA32F
};

struct ColorAttachmentSpecification
{
    ColorFormat format = ColorFormat::RGBA;
    TextureFilter filter = TextureFilter::LINEAR;
};

struct FramebufferSpecification
{
    ivec2 dimensions = ivec2(1);
    std::vector<ColorAttachmentSpecification> attachments = {};
    bool enableDepth = false;
};

#endif

struct FramebufferSpecification
{
    struct Attachment
    {
        enum class Index { FIRST, SECOND, THIRD, FOURTH, };

        std::optional<TextureSpecification> specification;
        ITexture::Ref texture = nullptr;
    };

    u32 width = 1u;
    u32 height = 1u;
    std::shared_ptr<IRenderPass> renderPass;
    std::vector<Attachment> attachments;
};

struct IFramebuffer: public Object
{
    BIND_OBJECT(IFramebuffer);
    virtual ~IFramebuffer() = default;
    
    virtual ITexture::Ref GetColorAttachment(FramebufferSpecification::Attachment::Index index) = 0;
};
