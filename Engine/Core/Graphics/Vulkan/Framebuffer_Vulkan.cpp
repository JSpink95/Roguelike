#include "Graphics\Vulkan\Framebuffer_Vulkan.h"

#ifdef _VULKAN
#include "Graphics\Vulkan\Context_Vulkan.h"
#include "Graphics\Vulkan\Texture_Vulkan.h"
#include "Graphics\Vulkan\RenderPass_Vulkan.h"

namespace vk
{
    Framebuffer_Vulkan::Ref Framebuffer_Vulkan::Create(
          std::shared_ptr<Context> context
        , FramebufferSpecification const& spec)
    {
        auto result = std::make_shared<Framebuffer_Vulkan>(context);

        if (!result->InitialiseFramebuffer(context, spec))
        {
            return nullptr;
        }

        return result;
    }
    
    Framebuffer_Vulkan::Framebuffer_Vulkan(std::shared_ptr<Context> context)
        : context(context)
    {

    }

    Framebuffer_Vulkan::~Framebuffer_Vulkan()
    {
        if (framebuffer != VK_NULL_HANDLE)
        {
            vkDestroyFramebuffer(context->device, framebuffer, nullptr);
        }
    }
    
    ITexture::Ref Framebuffer_Vulkan::GetColorAttachment(FramebufferSpecification::Attachment::Index index)
    {
        return nullptr;
    }

    bool Framebuffer_Vulkan::InitialiseFramebuffer(
          std::shared_ptr<Context> context
        , FramebufferSpecification const& spec)
    {
        auto renderPass = RenderPass_Vulkan::Cast(spec.renderPass);

        // TODO(jack): create new imageView if not already exist
        std::vector<VkImageView> imageViewAttachments;
        for (auto& attachment : spec.attachments)
        {
            Texture::Ref texture = Texture::Cast(attachment.texture);
            imageViewAttachments.push_back(texture->imageView);
        }

        VkFramebufferCreateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        createInfo.renderPass = renderPass->renderPass;
        createInfo.attachmentCount = (u32)imageViewAttachments.size();
        createInfo.pAttachments = imageViewAttachments.data();
        createInfo.width = spec.width;
        createInfo.height = spec.height;
        createInfo.layers = 1;
        
        if (vkCreateFramebuffer(context->device, &createInfo, nullptr, &framebuffer) != VK_SUCCESS)
        {
            LOG_ERROR("Failed to create a framebuffer!");
            return false;
        }

        return true;
    }
}
#endif