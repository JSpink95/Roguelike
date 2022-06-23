#pragma once
#include "Core.h"
#include "Graphics\Framebuffer.h"
#include "Graphics\OpenGL\CoreGL.h"

namespace gl
{
    struct GraphicsAPI;
    struct Texture;

    struct Framebuffer : IFramebuffer
    {
        BIND_OBJECT(Framebuffer);
        
        static Ref Create(FramebufferSpecification const& specification);

        Framebuffer(u32 width, u32 height);
        virtual ~Framebuffer();

        ITexture::Ref GetColorAttachment(FramebufferSpecification::Attachment::Index index) final;

        GLuint id = 0;
        ivec2 dimensions = ivec2(1);
        std::vector<std::shared_ptr<Texture>> colorAttachments;
        GLuint depthAttachment = 0;

    private:
        bool InitialiseFramebuffer(FramebufferSpecification const& specification);
    };
}