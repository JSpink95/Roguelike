#pragma once
#include "Core.h"

#ifdef _VULKAN
#include "Graphics\ShaderProgram.h"

#include <vulkan\vulkan.hpp>
#include <vector>

namespace vk
{
    struct Context;
    struct SwapChain;
    struct Buffer_Vulkan;

    struct ShaderTransferBlock : public IShaderTransferBlock
    {
        BIND_OBJECT(ShaderTransferBlock);

        static Ref Create(
              std::shared_ptr<Context> context
            , std::shared_ptr<SwapChain> swapChain
            , u64 binding
            , u64 blobSize
            , void const* blob);

        ShaderTransferBlock(
              std::shared_ptr<Context> context
            , std::shared_ptr<SwapChain> swapChain);

        void Set(u64 blobSize, void const* blob) final;

        VkDescriptorPool descriptorPool = VK_NULL_HANDLE;
        VkDescriptorSetLayout descriptorSetLayout = VK_NULL_HANDLE;
        VkPipelineLayout pipelineLayout = VK_NULL_HANDLE;
        std::vector<VkDescriptorSet> descriptorSets = {};
        std::vector<std::shared_ptr<Buffer_Vulkan>> uniformBuffers = {};
    private:
        bool InitialiseDescriptorSetLayout(u64 binding);
        bool InitialiseDescriptorPool();
        bool InitialiseUniformBuffers(
              u64 binding
            , u64 blobSize
            , void const* blob);

        std::shared_ptr<Context> context = nullptr;
        std::shared_ptr<SwapChain> swapChain = nullptr;
    };

    struct ShaderProgram : public IShaderProgram
    {
        BIND_OBJECT(ShaderProgram);
        static Ref Create(
              std::shared_ptr<Context> context
            , std::vector<Shader> const& shaders);

        void Begin() final;
        void End() final;

        void UploadTexture(std::shared_ptr<ITexture> tex, u64 binding) final;

        std::vector<VkShaderModule> shaderModules;
        std::vector<VkPipelineShaderStageCreateInfo> shaderStages;
    private:
        bool InitialiseShaderModules(
              std::shared_ptr<Context> context
            , std::vector<Shader> const& shaders);
    };
}
#endif