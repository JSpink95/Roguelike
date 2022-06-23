#pragma once
#include "Core.h"

#ifdef _VULKAN
#include "Graphics\RenderPass.h"
#include "Graphics\Vulkan\CoreVK.h"

namespace vk
{
    struct RenderPass_Vulkan : public IRenderPass
    {
        BIND_OBJECT(RenderPass_Vulkan);

        static Ref Create(
              std::shared_ptr<Context> context
            , std::shared_ptr<SwapChain> swapChain
            , RenderPassSpecification const& spec);

        RenderPass_Vulkan(
              std::shared_ptr<Context> context
            , std::shared_ptr<SwapChain> swapChain
            , RenderPassSpecification const& spec);
        ~RenderPass_Vulkan();

        ICommand::Ref Begin(IFramebuffer::Ref framebuffer) final;
        void End() final;

        IFramebuffer::Ref CreateFramebuffer() final;
        IFramebuffer::Ref CreateFramebuffer(std::vector<FramebufferSpecification::Attachment> const& attachments) final;

        std::shared_ptr<Context> context = nullptr;
        std::shared_ptr<SwapChain> swapChain = nullptr;
        RenderPassSpecification specification;

        VkRenderPass renderPass = VK_NULL_HANDLE;
        VkCommandBuffer commandBuffer = VK_NULL_HANDLE;
        IFramebuffer::Ref framebuffer = nullptr;
    private:
        bool InitialiseRenderPass();
        bool InitialiseCommandBuffer();
    };
}
#endif