#include "Graphics\Vulkan\GraphicsAPI_Vulkan.h"

#ifdef _VULKAN
#include "Core\window.h"

#include "Graphics\Vulkan\ShaderProgram_Vulkan.h"
#include "Graphics\Vulkan\Pipeline_Vulkan.h"
#include "Graphics\Vulkan\RenderPass_Vulkan.h"
#include "Graphics\Vulkan\Framebuffer_Vulkan.h"
#include "Graphics\Vulkan\CoreVK.h"

#include <set>
#include <optional>

namespace vk
{
    GraphicsAPI::GraphicsAPI()
    {
    }
    
    GraphicsAPI::~GraphicsAPI()
    {
        vkDestroySemaphore(context->device, renderFinishedSemaphore, nullptr);
        vkDestroySemaphore(context->device, imageAvailableSemaphore, nullptr);
    }
    
    void GraphicsAPI::Initialize(std::shared_ptr<AppWindow> window)
    {
        context = Context::Create(window);

        swapChain = SwapChain::Create(context, window);

        RenderPassSpecification specification;
        specification.size = { swapChain->extent.width, swapChain->extent.height };
        specification.attachments.push_back({ RenderPassSpecification::Attachment::Format::RGBA, RenderPassSpecification::Attachment::Type::COLOR, 1u });

        defaultRenderPass = RenderPass_Vulkan::Create(context, swapChain, specification);
        swapChain->InitialiseFramebuffers(RenderPass_Vulkan::Cast(defaultRenderPass));

        InitialiseSemaphores();
    }

    ICommand::Ref GraphicsAPI::BeginFrame()
    {
        return defaultRenderPass->Begin(swapChain->framebuffers.at(swapChain->activeFramebufferIndex));
    }

    void GraphicsAPI::FinishFrame()
    {
        defaultRenderPass->End();

        u32 imageIndex;
        vkAcquireNextImageKHR(context->device, swapChain->swapChain, UINT64_MAX, imageAvailableSemaphore, VK_NULL_HANDLE, &imageIndex);
        
        VkSubmitInfo submitInfo = {};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

        VkSemaphore waitSemaphores[] = { imageAvailableSemaphore };
        VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
        submitInfo.waitSemaphoreCount = 1;
        submitInfo.pWaitSemaphores = waitSemaphores;
        submitInfo.pWaitDstStageMask = waitStages;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &RenderPass_Vulkan::Cast(defaultRenderPass)->commandBuffer;

        VkSemaphore signalSemaphores[] = { renderFinishedSemaphore };
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores = signalSemaphores;

        VkResult result = vkQueueSubmit(context->graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
        if (result != VK_SUCCESS)
        {
            LOG_WARNING("Failed to submit draw command buffer(%lli)!\n", (long long)result);
        }

        VkSubpassDependency dependency = {};
        dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
        dependency.dstSubpass = 0;
        dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependency.srcAccessMask = 0;
        dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

        //renderPassInfo.dependencyCount = 1;
        //renderPassInfo.pDependencies = &dependency;

        VkPresentInfoKHR presentInfo = {};
        presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        presentInfo.waitSemaphoreCount = 1;
        presentInfo.pWaitSemaphores = signalSemaphores;

        VkSwapchainKHR swapChains[] = { swapChain->swapChain };
        presentInfo.swapchainCount = 1;
        presentInfo.pSwapchains = swapChains;
        presentInfo.pImageIndices = &imageIndex;
        presentInfo.pResults = nullptr; // Optional

        vkQueuePresentKHR(context->presentQueue, &presentInfo);
        swapChain->activeFramebufferIndex = (swapChain->activeFramebufferIndex + 1) % swapChain->framebuffers.size();
    }
    
    void GraphicsAPI::InitialiseSemaphores()
    {
        VkSemaphoreCreateInfo semaphoreInfo{};
        semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

        if (vkCreateSemaphore(context->device, &semaphoreInfo, nullptr, &imageAvailableSemaphore) != VK_SUCCESS ||
            vkCreateSemaphore(context->device, &semaphoreInfo, nullptr, &renderFinishedSemaphore) != VK_SUCCESS)
        {
            LOG_ERRROR("Failed to create semaphores!\n");
        }
    }

    IShaderProgram::Ref GraphicsAPI::CreateShaderProgram(
          std::string_view path
        , ShaderProgramConfig const& config)
    {
        return nullptr;
    }

    IShaderProgram::Ref GraphicsAPI::CreateShaderProgram(
        std::vector<Shader> const& shaders)
    {
        return ShaderProgram::Create(context, shaders);
    }

    IShaderTransferBlock::Ref GraphicsAPI::CreateShaderTransferBlock(
          u64 blobSize
        , void const* blob
        , u64 binding)
    {
        return ShaderTransferBlock::Create(context, swapChain, binding, blobSize, blob);
    }

    IPipeline::Ref GraphicsAPI::CreatePipeline(
        PipelineSpecification const& spec)
    {
        return Pipeline_Vulkan::Create(context, swapChain, spec);
    }

    IRenderPass::Ref GraphicsAPI::CreateRenderPass(
        RenderPassSpecification const& spec)
    {
        return RenderPass_Vulkan::Create(context, swapChain, spec);
    }
    
    IRenderPass::Ref GraphicsAPI::GetDefaultRenderPass()
    {
        return defaultRenderPass;
    }

    ITexture::Ref GraphicsAPI::CreateTexture(
          int width
        , int height
        , int components
        , PrimitiveType type
        , void* pixels
        , TextureSpecification const& spec)
    {
        return nullptr;
    }

    ITexture::Ref GraphicsAPI::CreateTexture(
          std::string_view path
        , TextureSpecification const& spec)
    {
        return nullptr;
    }
}
#endif