#include "Graphics\Vulkan\ShaderProgram_Vulkan.h"

#ifdef _VULKAN
#include "Graphics\Vulkan\Context_Vulkan.h"
#include "Graphics\Vulkan\Vertex_Vulkan.h"
#include "Graphics\Vulkan\SwapChain_Vulkan.h"

#include "Platform\FileIO.h"

namespace vk
{
    namespace utils
    {
        VkShaderStageFlagBits FromShaderType(u64 type)
        {
            switch (type)
            {
                case ShaderFlags::VERTEX:
                    return VK_SHADER_STAGE_VERTEX_BIT;
                case ShaderFlags::FRAGMENT:
                    return VK_SHADER_STAGE_FRAGMENT_BIT;
                case ShaderFlags::GEOMETRY:
                    return VK_SHADER_STAGE_GEOMETRY_BIT;
                default:
                    return VK_SHADER_STAGE_FLAG_BITS_MAX_ENUM;
            }
        }
    }

    ShaderTransferBlock::Ref ShaderTransferBlock::Create(
          std::shared_ptr<Context> context
        , std::shared_ptr<SwapChain> swapChain
        , u64 binding
        , u64 blobSize
        , void const* blob)
    {
        auto result = std::make_shared<ShaderTransferBlock>(context, swapChain);

        if (!result->InitialiseDescriptorPool())
        {
            return nullptr;
        }

        if (!result->InitialiseDescriptorSetLayout(binding))
        {
            return nullptr;
        }

        if (!result->InitialiseUniformBuffers(binding, blobSize, blob))
        {
            return nullptr;
        }

        return result;
    }

    ShaderTransferBlock::ShaderTransferBlock(std::shared_ptr<Context> context, std::shared_ptr<SwapChain> swapChain)
        : context(context)
        , swapChain(swapChain)
    {

    }

    void ShaderTransferBlock::Set(u64 blobSize, void const* blob)
    {
        uniformBuffers.at(swapChain->activeFramebufferIndex)->Set(blobSize, blob);
        //for (auto uniformBuffer : uniformBuffers)
        //    uniformBuffer->Set(blobSize, blob);
    }

    bool ShaderTransferBlock::InitialiseDescriptorPool()
    {
        VkDescriptorPoolSize poolSize = {};
        poolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        poolSize.descriptorCount = static_cast<uint32_t>(swapChain->images.size());

        VkDescriptorPoolCreateInfo poolInfo = {};
        poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        poolInfo.poolSizeCount = 1;
        poolInfo.pPoolSizes = &poolSize;
        poolInfo.maxSets = static_cast<uint32_t>(swapChain->images.size());

        if (vkCreateDescriptorPool(context->device, &poolInfo, nullptr, &descriptorPool) != VK_SUCCESS)
        {
            LOG_ERROR("Failed to create descriptor pool!\n");
            return false;
        }

        return true;
    }

    bool ShaderTransferBlock::InitialiseDescriptorSetLayout(u64 binding)
    {
        VkDescriptorSetLayoutBinding uboLayoutBinding = {};
        uboLayoutBinding.binding = (u32)binding;
        uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        uboLayoutBinding.descriptorCount = 1;
        uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT; // TODO(jack): move to config
        uboLayoutBinding.pImmutableSamplers = nullptr; // Optional

        VkDescriptorSetLayoutCreateInfo layoutInfo = {};
        layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        layoutInfo.bindingCount = 1;
        layoutInfo.pBindings = &uboLayoutBinding;

        if (vkCreateDescriptorSetLayout(context->device, &layoutInfo, nullptr, &descriptorSetLayout) != VK_SUCCESS)
        {
            LOG_ERROR("Failed to create descriptor set layout!\n");
            return false;
        }

        std::vector<VkDescriptorSetLayout> layouts(swapChain->images.size(), descriptorSetLayout);

        VkDescriptorSetAllocateInfo allocInfo = {};
        allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        allocInfo.descriptorPool = descriptorPool;
        allocInfo.descriptorSetCount = static_cast<uint32_t>(swapChain->images.size());
        allocInfo.pSetLayouts = layouts.data();

        // one descriptor per swapchain image
        descriptorSets.resize(swapChain->images.size());
        if (vkAllocateDescriptorSets(context->device, &allocInfo, descriptorSets.data()) != VK_SUCCESS)
        {
            LOG_ERROR("Failed to allocate descriptor sets!\n");
            return false;
        }

        return true;
    }

    bool ShaderTransferBlock::InitialiseUniformBuffers(
          u64 binding
        , u64 blobSize
        , void const* blob)
    {
        for (u32 descriptorIndex = 0u; descriptorIndex < descriptorSets.size(); ++descriptorIndex)
        {
            auto uniformBuffer = uniformBuffers.emplace_back(Buffer_Vulkan::Create(context, BufferType::UNIFORM, blobSize, blob));

            VkDescriptorBufferInfo bufferInfo = {};
            bufferInfo.buffer = uniformBuffer->buffer;
            bufferInfo.offset = 0;
            bufferInfo.range = blobSize;

            VkWriteDescriptorSet descriptorWrite = {};
            descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptorWrite.dstSet = descriptorSets.at(descriptorIndex);
            descriptorWrite.dstBinding = binding;
            descriptorWrite.dstArrayElement = 0;
            descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            descriptorWrite.descriptorCount = 1;
            descriptorWrite.pBufferInfo = &bufferInfo;
            descriptorWrite.pImageInfo = nullptr; // Optional
            descriptorWrite.pTexelBufferView = nullptr; // Optional

            vkUpdateDescriptorSets(context->device, 1, &descriptorWrite, 0, nullptr);
        }

        return true;
    }

    ShaderProgram::Ref ShaderProgram::Create(
          std::shared_ptr<Context> context
        , std::vector<Shader> const& shaders)
    {
        auto program = std::make_shared<ShaderProgram>();

        if (!program->InitialiseShaderModules(context, shaders))
        {
            return nullptr;
        }

        return program;
    }

    bool ShaderProgram::InitialiseShaderModules(
          std::shared_ptr<Context> context
        , std::vector<Shader> const& shaders)
    {
        for (auto shader : shaders)
        {
            auto bytecode = core::fileio::LoadBinaryFile(shader.path);

            VkShaderModuleCreateInfo createInfo = {};
            createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
            createInfo.codeSize = bytecode.size();
            createInfo.pCode = reinterpret_cast<const u32*>(bytecode.data());

            VkShaderModule shaderModule;
            if (vkCreateShaderModule(context->device, &createInfo, nullptr, &shaderModule) != VK_SUCCESS)
            {
                LOG_ERROR("Failed to create ShaderModule \"%s\"!\n", shader.path);
                return false;
            }

            shaderModules.push_back(shaderModule);

            VkPipelineShaderStageCreateInfo& shaderStageInfo = shaderStages.emplace_back();
            shaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
            shaderStageInfo.stage = utils::FromShaderType(shader.type);
            shaderStageInfo.module = shaderModule;
            shaderStageInfo.pName = "main";
        }

        return true;
    }

    void ShaderProgram::Begin()
    {

    }

    void ShaderProgram::End()
    {

    }

    void ShaderProgram::UploadTexture(std::shared_ptr<ITexture> tex, u64 binding)
    {

    }
}
#endif