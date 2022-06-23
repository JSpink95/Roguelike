#pragma once
#include "Core.h"
#include "Graphics\RenderPass.h"

namespace gl
{
    struct GraphicsAPI;

    struct RenderPass : public IRenderPass
    {
        BIND_OBJECT(RenderPass);

        static Ref Create(
              std::shared_ptr<GraphicsAPI> graphics
            , RenderPassSpecification const& spec);

        RenderPass(
              std::shared_ptr<GraphicsAPI> graphics
            , RenderPassSpecification const& spec);

        ICommand::Ref Begin(IFramebuffer::Ref framebuffer) final;
        void End() final;

        IFramebuffer::Ref CreateFramebuffer() final;
        IFramebuffer::Ref CreateFramebuffer(std::vector<FramebufferSpecification::Attachment> const& attachments) final;

        RenderPassSpecification specification;
        std::shared_ptr<GraphicsAPI> graphics;
    };
}