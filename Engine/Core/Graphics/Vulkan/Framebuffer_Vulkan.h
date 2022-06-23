#pragma once
#include "Core.h"

#ifdef _VULKAN
#include "Graphics\Framebuffer.h"
#include "Graphics\Vulkan\CoreVK.h"

namespace vk
{
    struct Context;
    struct Framebuffer_Vulkan : public IFramebuffer
    {
        BIND_OBJECT(Framebuffer_Vulkan);

        static Ref Create(
              std::shared_ptr<Context> context
            , FramebufferSpecification const& spec);

        Framebuffer_Vulkan(std::shared_ptr<Context> context);
        virtual ~Framebuffer_Vulkan();

        ITexture::Ref GetColorAttachment(FramebufferSpecification::Attachment::Index index) final;

        std::shared_ptr<Context> context;
        std::vector<ITexture::Ref> attachmentTextures;
        VkFramebuffer framebuffer;

    private:
        bool InitialiseFramebuffer(
              std::shared_ptr<Context> context
            , FramebufferSpecification const& spec);
    };
}
#endif