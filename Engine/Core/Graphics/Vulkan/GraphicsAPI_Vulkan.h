#pragma once
#include "Core.h"

#ifdef _VULKAN
#include "Graphics\GraphicsAPI.h"
#include "Graphics\Vulkan\CoreVK.h"
#include "Graphics\Vulkan\Context_Vulkan.h"
#include "Graphics\Vulkan\SwapChain_Vulkan.h"
#include "Graphics\Vulkan\Command_Vulkan.h"

#include <vulkan\vulkan.hpp>

namespace vk
{
    struct GraphicsAPI : public IGraphicsDevice
    {
        BIND_OBJECT(GraphicsAPI);

        GraphicsAPI();
        ~GraphicsAPI();

        void Initialize(std::shared_ptr<AppWindow> window) final;

        IShaderProgram::Ref CreateShaderProgram(
              std::string_view path
            , ShaderProgramConfig const& config) final;

        IShaderProgram::Ref CreateShaderProgram(
            std::vector<Shader> const& shaders) final;

        IShaderTransferBlock::Ref CreateShaderTransferBlock(
              u64 blobSize
            , void const* blob
            , u64 binding) final;

        // pipeline creation
        IPipeline::Ref CreatePipeline(
            PipelineSpecification const& spec) final;

        // render pass
        IRenderPass::Ref CreateRenderPass(
            RenderPassSpecification const& spec) final;

        IRenderPass::Ref GetDefaultRenderPass() final;
        
        // texture creation
        ITexture::Ref CreateTexture(
              int width
            , int height
            , int components
            , PrimitiveType type
            , void* pixels
            , TextureSpecification const& spec) final;

        ITexture::Ref CreateTexture(
              std::string_view path
            , TextureSpecification const& spec) final;

        ICommand::Ref BeginFrame() final;
        void FinishFrame() final;

        void InitialiseSemaphores();

        Context::Ref context = nullptr;
        SwapChain::Ref swapChain = nullptr;
        IRenderPass::Ref defaultRenderPass = nullptr;

        VkSemaphore imageAvailableSemaphore = VK_NULL_HANDLE;
        VkSemaphore renderFinishedSemaphore = VK_NULL_HANDLE;
    };
}
#endif