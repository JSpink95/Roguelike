#include "Graphics\Vulkan\RenderPass_Vulkan.h"

#ifdef _VULKAN
#include "Graphics\Vulkan\GraphicsAPI_Vulkan.h"
#include "Graphics\Vulkan\Framebuffer_Vulkan.h"
#include "Graphics\Vulkan\Texture_Vulkan.h"
#include "Graphics\Vulkan\Command_Vulkan.h"

namespace vk
{
    namespace utils
    {
        VkFormat FromAttachmentFormat(RenderPassSpecification::Attachment::Format format)
        {
            switch (format)
            {
                case RenderPassSpecification::Attachment::Format::RED:
                    return VkFormat::VK_FORMAT_R8_UNORM;

                case RenderPassSpecification::Attachment::Format::RGB:
                    return VkFormat::VK_FORMAT_R8G8B8_UNORM;

                case RenderPassSpecification::Attachment::Format::RGBA:
                    return VkFormat::VK_FORMAT_R8G8B8A8_UNORM;

                default:
                    return VkFormat::VK_FORMAT_UNDEFINED;
            }
        }

        VkSampleCountFlagBits FromSamples(u32 samples)
        {
            switch (samples)
            {
                case 1:
                    return VkSampleCountFlagBits::VK_SAMPLE_COUNT_1_BIT;
                default:
                    return VkSampleCountFlagBits::VK_SAMPLE_COUNT_1_BIT;
            }
        }
    }

    RenderPass_Vulkan::Ref RenderPass_Vulkan::Create(
          std::shared_ptr<Context> context
        , std::shared_ptr<SwapChain> swapChain
        , RenderPassSpecification const& spec)
    {
        auto result = std::make_shared<RenderPass_Vulkan>(context, swapChain, spec);

        if (!result->InitialiseRenderPass())
        {
            return nullptr;
        }

        if (!result->InitialiseCommandBuffer())
        {
            return nullptr;
        }

        return result;
    }

    RenderPass_Vulkan::RenderPass_Vulkan(std::shared_ptr<Context> context, std::shared_ptr<SwapChain> swapChain, RenderPassSpecification const& spec)
        : context(context)
        , swapChain(swapChain)
        , specification(spec)
    {

    }

    RenderPass_Vulkan::~RenderPass_Vulkan()
    {
        vkDestroyRenderPass(context->device, renderPass, nullptr);
    }
    
    ICommand::Ref RenderPass_Vulkan::Begin(IFramebuffer::Ref framebuffer)
    {
        // we can pass a null framebuffer here, if we do then we want to render to the active screen
        auto fb = Framebuffer_Vulkan::Cast(framebuffer ? framebuffer : swapChain->GetActiveFramebuffer());

        VkRenderPassBeginInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = renderPass;
        renderPassInfo.framebuffer = fb->framebuffer;
        renderPassInfo.renderArea.offset = { 0, 0 };
        renderPassInfo.renderArea.extent = { 1904, 1001 }; // TODO(jack): get render size

        VkClearValue clearColor = { { { 0.0f, 0.0f, 0.0f, 1.0f } } };
        renderPassInfo.clearValueCount = 1;
        renderPassInfo.pClearValues = &clearColor;

        VkCommandBufferBeginInfo commandBeginInfo = {};
        commandBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        commandBeginInfo.flags = 0; // Optional
        commandBeginInfo.pInheritanceInfo = nullptr; // Optional

        vkBeginCommandBuffer(commandBuffer, &commandBeginInfo);
        vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

        return Command::Create(commandBuffer, swapChain->activeFramebufferIndex);
    }

    void RenderPass_Vulkan::End()
    {
        vkCmdEndRenderPass(commandBuffer);
        vkEndCommandBuffer(commandBuffer);
    }
    
    IFramebuffer::Ref RenderPass_Vulkan::CreateFramebuffer()
    {
        FramebufferSpecification framebufferSpecification = {};
        framebufferSpecification.width = specification.size.x;
        framebufferSpecification.height = specification.size.y;
        framebufferSpecification.renderPass = GetRenderPass_VulkanRef();

        for (auto& attachment : specification.attachments)
        {
            TextureSpecification textureSpecification;

            FramebufferSpecification::Attachment& framebufferAttachment = framebufferSpecification.attachments.emplace_back();
            framebufferAttachment.specification = textureSpecification;
        }

        return Framebuffer_Vulkan::Create(context, framebufferSpecification);
    }

    std::shared_ptr<IFramebuffer> RenderPass_Vulkan::CreateFramebuffer(std::vector<FramebufferSpecification::Attachment> const& attachments)
    {
        FramebufferSpecification framebufferSpecification = {};
        framebufferSpecification.width = specification.size.x;
        framebufferSpecification.height = specification.size.y;
        framebufferSpecification.renderPass = GetRenderPass_VulkanRef();
        framebufferSpecification.attachments = attachments;

        return Framebuffer_Vulkan::Create(context, framebufferSpecification);
    }

    bool RenderPass_Vulkan::InitialiseRenderPass()
    {
        u32 attachmentIndex = 0u;

        std::vector<VkAttachmentDescription> attachmentDescriptions;
        std::vector<VkAttachmentReference> attachmentReferences;

        for (auto& attachment : specification.attachments)
        {
            VkAttachmentDescription& attachmentDescription = attachmentDescriptions.emplace_back();
            attachmentDescription.format = utils::FromAttachmentFormat(attachment.format);
            attachmentDescription.samples = utils::FromSamples(attachment.samples);
            attachmentDescription.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
            attachmentDescription.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
            attachmentDescription.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
            attachmentDescription.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
            attachmentDescription.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
            attachmentDescription.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

            VkAttachmentReference& attachmentRef = attachmentReferences.emplace_back();
            attachmentRef.attachment = attachmentIndex;
            attachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

            ++attachmentIndex;
        }

        VkSubpassDescription subpass = {};
        subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
        subpass.colorAttachmentCount = attachmentReferences.size();
        subpass.pColorAttachments = attachmentReferences.data();

        VkRenderPassCreateInfo renderPassInfo = {};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        renderPassInfo.attachmentCount = 1;
        renderPassInfo.pAttachments = attachmentDescriptions.data();
        renderPassInfo.subpassCount = 1;
        renderPassInfo.pSubpasses = &subpass;

        if (vkCreateRenderPass(context->device, &renderPassInfo, nullptr, &renderPass) != VK_SUCCESS)
        {
            LOG_ERROR("Failed to create render pass!\n");
            return false;
        }

        return true;
    }

    bool RenderPass_Vulkan::InitialiseCommandBuffer()
    {
        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.commandPool = context->commandPool;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandBufferCount = 1u;

        if (vkAllocateCommandBuffers(context->device, &allocInfo, &commandBuffer) != VK_SUCCESS)
        {
            LOG_ERROR("Failed to allocate command buffers!\n");
            return false;
        }

        return true;
    }
}
#endif