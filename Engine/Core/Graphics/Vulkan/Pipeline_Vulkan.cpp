#include "Graphics\Vulkan\Pipeline_Vulkan.h"

#ifdef _VULKAN
#include "Graphics\Vulkan\Context_Vulkan.h"
#include "Graphics\Vulkan\SwapChain_Vulkan.h"
#include "Graphics\Vulkan\Vertex_Vulkan.h"
#include "Graphics\Vulkan\Command_Vulkan.h"
#include "Graphics\Vulkan\ShaderProgram_Vulkan.h"
#include "Graphics\Vulkan\RenderPass_Vulkan.h"
#include "Graphics\Vulkan\Framebuffer_Vulkan.h"

namespace vk
{
    namespace utils
    {
        VkFormat GetFormatFromAttributeType(VertexAttributeType type)
        {
            switch (type)
            {

                case VertexAttributeType::INT:
                    return VkFormat::VK_FORMAT_R32_SINT;
                case VertexAttributeType::INT2:
                    return VkFormat::VK_FORMAT_R32G32_SINT;
                case VertexAttributeType::INT3:
                    return VkFormat::VK_FORMAT_R32G32B32_SINT;
                case VertexAttributeType::INT4:
                    return VkFormat::VK_FORMAT_R32G32B32A32_SINT;
                case VertexAttributeType::FLOAT:
                    return VkFormat::VK_FORMAT_R32_SFLOAT;
                case VertexAttributeType::FLOAT2:
                    return VkFormat::VK_FORMAT_R32G32_SFLOAT;
                case VertexAttributeType::FLOAT3:
                    return VkFormat::VK_FORMAT_R32G32B32_SFLOAT;
                case VertexAttributeType::FLOAT4:
                    return VkFormat::VK_FORMAT_R32G32B32A32_SFLOAT;
                case VertexAttributeType::MAT4:
                    return VkFormat::VK_FORMAT_UNDEFINED;
            }
        }

        VkPrimitiveTopology FromTopologyType(TopologyMode mode)
        {
            switch (mode)
            {
                case TopologyMode::LINES:
                    return VkPrimitiveTopology::VK_PRIMITIVE_TOPOLOGY_LINE_LIST;
                case TopologyMode::TRIANGLES:
                    return VkPrimitiveTopology::VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
            }
        }
    }
    Pipeline_Vulkan::Ref Pipeline_Vulkan::Create(
          std::shared_ptr<Context> context
        , std::shared_ptr<SwapChain> swapChain
        , PipelineSpecification const& spec)
    {
        auto result = std::make_shared<Pipeline_Vulkan>(context, swapChain, spec);

        if (!result->InitialisePipelineLayout())
        {
            return nullptr;
        }

        if (!result->InitialisePipeline())
        {
            return nullptr;
        }

        return result;
    }

    Pipeline_Vulkan::Pipeline_Vulkan(std::shared_ptr<Context> context, std::shared_ptr<SwapChain> swapChain, PipelineSpecification const& spec)
        : context(context)
        , swapChain(swapChain)
        , specification(spec)
    {

    }

    void Pipeline_Vulkan::Submit(
          ICommand::Ref comm
        , std::shared_ptr<IVertexArray> mesh)
    {
        auto vkMesh = VertexArray::Cast(mesh);

        VkBuffer vertexBuffers[] = { vkMesh->vertexBuffer->buffer };
        VkDeviceSize offsets[] = { 0 };

        auto command = Command::Cast(comm);

        std::vector<VkDescriptorSet> descriptorSets = {};
        for (auto descriptor : specification.descriptors)
            descriptorSets.push_back(ShaderTransferBlock::Cast(descriptor)->descriptorSets.at(command->activeImageIndex));

        vkCmdBindPipeline(command->commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);
        vkCmdBindDescriptorSets(command->commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, descriptorSets.size(), descriptorSets.data(), 0, nullptr);
        vkCmdBindVertexBuffers(command->commandBuffer, 0, 1, vertexBuffers, offsets);
        vkCmdDraw(command->commandBuffer, vkMesh->vertexCount, 1, 0, 0);
    }

    std::shared_ptr<IVertexArray> Pipeline_Vulkan::CreateVertexArray(
          u64 vertexCount
        , void const* vertices) 
    {
        return VertexArray::Create(context, vertexCount, specification.stride(), vertices);
    }
    
    bool Pipeline_Vulkan::InitialisePipelineLayout()
    {
        std::vector<VkDescriptorSetLayout> descriptorLayouts = {};
        for (auto descriptorLayout : specification.descriptors)
            descriptorLayouts.push_back(ShaderTransferBlock::Cast(descriptorLayout)->descriptorSetLayout);

        VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount = descriptorLayouts.size();
        pipelineLayoutInfo.pSetLayouts = descriptorLayouts.data();
        pipelineLayoutInfo.pushConstantRangeCount = 0; // Optional
        pipelineLayoutInfo.pPushConstantRanges = nullptr; // Optional

        if (vkCreatePipelineLayout(context->device, &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS)
        {
            LOG_ERROR("Failed to create pipeline layout!\n");
            return false;
        }

        return true;
    }

    bool Pipeline_Vulkan::InitialisePipeline()
    {
        auto shader = ShaderProgram::Cast(specification.shader);
        auto renderPass = RenderPass_Vulkan::Cast(specification.renderPass);

        u64 vertexStride = 0u;
        std::vector<VkVertexInputAttributeDescription> vertexInputAttributeDescriptions = {};

        for (u32 attributeIndex = 0u; attributeIndex < specification.input.size(); ++attributeIndex)
        {
            VertexAttributeType attribute = specification.input.at(attributeIndex);

            VkVertexInputAttributeDescription& attributeDescription = vertexInputAttributeDescriptions.emplace_back();
            attributeDescription.binding = 0;
            attributeDescription.location = attributeIndex;
            attributeDescription.format = utils::GetFormatFromAttributeType(attribute);
            attributeDescription.offset = vertexStride;

            vertexStride += core::AttributeTypeToByteCount(attribute);
        }

        VkVertexInputBindingDescription vertexInputDescription = {};
        vertexInputDescription.binding = 0;
        vertexInputDescription.stride = (u32)vertexStride;
        vertexInputDescription.inputRate = VkVertexInputRate::VK_VERTEX_INPUT_RATE_VERTEX; // per vertex data

        VkPipelineVertexInputStateCreateInfo vertexInputInfo = {};
        vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        vertexInputInfo.vertexBindingDescriptionCount = 1;
        vertexInputInfo.pVertexBindingDescriptions = &vertexInputDescription;
        vertexInputInfo.vertexAttributeDescriptionCount = vertexInputAttributeDescriptions.size();
        vertexInputInfo.pVertexAttributeDescriptions = vertexInputAttributeDescriptions.data();

        // setup input assembly
        VkPipelineInputAssemblyStateCreateInfo inputAssemblyState = {};
        inputAssemblyState.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        inputAssemblyState.topology = utils::FromTopologyType(specification.topologyMode);
        inputAssemblyState.primitiveRestartEnable = VK_FALSE;

        // setup viewport state
        VkViewport viewport = {};
        viewport.x = 0.0f;
        viewport.y = (float)swapChain->extent.height;
        viewport.width = (float)swapChain->extent.width;
        viewport.height = -(float)swapChain->extent.height;
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;

        VkRect2D scissor = {};
        scissor.offset = { 0, 0 };
        scissor.extent = swapChain->extent;

        VkPipelineViewportStateCreateInfo viewportState = {};
        viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        viewportState.viewportCount = 1;
        viewportState.pViewports = &viewport;
        viewportState.scissorCount = 1;
        viewportState.pScissors = &scissor;

        // setup the rasterizer
        // TODO(jack): move this into the pipeline specification
        VkPipelineRasterizationStateCreateInfo rasterizer = {};
        rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        rasterizer.depthClampEnable = VK_FALSE;
        rasterizer.rasterizerDiscardEnable = VK_FALSE;
        rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
        rasterizer.lineWidth = 1.0f;
        rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
        rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
        rasterizer.depthBiasEnable = VK_FALSE;
        rasterizer.depthBiasConstantFactor = 0.0f; // Optional
        rasterizer.depthBiasClamp = 0.0f; // Optional
        rasterizer.depthBiasSlopeFactor = 0.0f; // Optional

        // setup the multisampler
        // TODO(jack): move this into the pipeline specification
        VkPipelineMultisampleStateCreateInfo multisampling = {};
        multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        multisampling.sampleShadingEnable = VK_FALSE;
        multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
        multisampling.minSampleShading = 1.0f; // Optional
        multisampling.pSampleMask = nullptr; // Optional
        multisampling.alphaToCoverageEnable = VK_FALSE; // Optional
        multisampling.alphaToOneEnable = VK_FALSE; // Optional

        // setup the colorblending
        // TODO(jack): move this into the pipeline specification
        VkPipelineColorBlendAttachmentState colorBlendAttachment = {};
        colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
        colorBlendAttachment.blendEnable = VK_TRUE;
        colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
        colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
        colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD; // Optional
        colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
        colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
        colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD; // Optional

        VkPipelineColorBlendStateCreateInfo colorBlending = {};
        colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        colorBlending.logicOpEnable = VK_FALSE;
        colorBlending.logicOp = VK_LOGIC_OP_COPY; // Optional
        colorBlending.attachmentCount = 1;
        colorBlending.pAttachments = &colorBlendAttachment;
        colorBlending.blendConstants[0] = 0.0f; // Optional
        colorBlending.blendConstants[1] = 0.0f; // Optional
        colorBlending.blendConstants[2] = 0.0f; // Optional
        colorBlending.blendConstants[3] = 0.0f; // Optional

        // TODO(jack): setup dynamic state configuration here
        // Dynamic states allow us to change certain details _without_ recreating the whole pipeline.

        VkPipelineDepthStencilStateCreateInfo depthStencilState = {};
        depthStencilState.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
        //depthStencilState.flags;
        depthStencilState.depthTestEnable = true;
        depthStencilState.depthWriteEnable = true;
        depthStencilState.depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL;
        depthStencilState.depthBoundsTestEnable = true;
        depthStencilState.stencilTestEnable = false;
        //depthStencilState.front;
        //depthStencilState.back;
        depthStencilState.minDepthBounds = 0.01f;
        depthStencilState.maxDepthBounds = 100.0f;

        VkGraphicsPipelineCreateInfo pipelineInfo = {};
        pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        pipelineInfo.stageCount = (u32)shader->shaderStages.size();
        pipelineInfo.pStages = shader->shaderStages.data();

        // fixed function passes
        pipelineInfo.pVertexInputState = &vertexInputInfo;
        pipelineInfo.pInputAssemblyState = &inputAssemblyState;
        pipelineInfo.pViewportState = &viewportState;
        pipelineInfo.pRasterizationState = &rasterizer;
        pipelineInfo.pMultisampleState = &multisampling;
        pipelineInfo.pDepthStencilState = &depthStencilState;
        pipelineInfo.pColorBlendState = &colorBlending;
        pipelineInfo.pDynamicState = nullptr;

        // pipeline layout
        pipelineInfo.layout = pipelineLayout;

        // render pass
        pipelineInfo.renderPass = renderPass->renderPass;
        pipelineInfo.subpass = 0;

        pipelineInfo.basePipelineHandle = VK_NULL_HANDLE; // Optional
        pipelineInfo.basePipelineIndex = -1; // Optional

        if (vkCreateGraphicsPipelines(context->device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &pipeline) != VK_SUCCESS)
        {
            LOG_ERROR("Failed to create graphics pipeline!\n");
            return false;
        }

        return true;
    }
}
#endif