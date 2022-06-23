#pragma once
#include "Core.h"
#include "Core\Object.h"
#include "Graphics\Framebuffer.h"
#include "Graphics\Command.h"

#include <vector>

struct RenderPassSpecification
{
    struct Attachment
    {
        enum class Format { RED, RGB, RGBA };
        enum class Type { COLOR, DEPTH, STENCIL, DEPTHSTENCIL };

        Format format = Format::RGBA;
        Type type = Type::COLOR;
        u32 samples = 1u;
    };

    ivec2 size = ivec2(1);
    std::vector<Attachment> attachments = {};
    vec4 clearColor = vec4(0.0f);

    RenderPassSpecification() = default;
    RenderPassSpecification(std::initializer_list<Attachment> inAttachments)
        : attachments(inAttachments) {}
};

struct IRenderPass : public Object
{
    BIND_OBJECT(IRenderPass);

    virtual ICommand::Ref Begin(IFramebuffer::Ref framebuffer) = 0;
    virtual void End() = 0;

    virtual IFramebuffer::Ref CreateFramebuffer() = 0;
    virtual IFramebuffer::Ref CreateFramebuffer(std::vector<FramebufferSpecification::Attachment> const& attachments) = 0;
};