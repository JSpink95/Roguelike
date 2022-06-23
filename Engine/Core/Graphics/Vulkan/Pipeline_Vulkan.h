#pragma once
#include "Core.h"

#ifdef _VULKAN
#include "Graphics\Pipeline.h"
#include "Graphics\ShaderProgram.h"

#include <vulkan\vulkan.hpp>

namespace vk
{
    struct Context;
    struct SwapChain;

    // NOTE(jack): the vulkan api has a vk::Pipeline already defined so I can't just call this Pipeline
    // as it causes a redefinition error

    struct Pipeline_Vulkan: public IPipeline
    {
        BIND_OBJECT(Pipeline_Vulkan);

        static Ref Create(
              std::shared_ptr<Context> context
            , std::shared_ptr<SwapChain> swapChain
            , PipelineSpecification const& spec);

        Pipeline_Vulkan(
              std::shared_ptr<Context> context
            , std::shared_ptr<SwapChain> swapChain
            , PipelineSpecification const& spec);

        void Submit(
              ICommand::Ref command
            , std::shared_ptr<IVertexArray> mesh) final;

        std::shared_ptr<IVertexArray> CreateVertexArray(
              u64 vertexCount
            , void const* vertices) final;

        std::shared_ptr<Context> context = nullptr;
        std::shared_ptr<SwapChain> swapChain = nullptr;
        PipelineSpecification specification = {};
        VkPipelineLayout pipelineLayout = VK_NULL_HANDLE;
        VkPipeline pipeline = VK_NULL_HANDLE;
    private:
        bool InitialisePipelineLayout();
        bool InitialisePipeline();
    };
}
#endif