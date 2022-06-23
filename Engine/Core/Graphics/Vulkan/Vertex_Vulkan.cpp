#include "Graphics\Vulkan\Vertex_Vulkan.h"

#ifdef _VULKAN
#include "Graphics\Vulkan\Context_Vulkan.h"

namespace vk
{
    namespace utils
    {
        VkBufferUsageFlags FromBufferType(BufferType type)
        {
            switch (type)
            {
                case BufferType::VERTEX:
                    return VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;

                case BufferType::INDEX:
                    return VK_BUFFER_USAGE_INDEX_BUFFER_BIT;

                case BufferType::UNIFORM:
                    return VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;

                default:
                    return VK_BUFFER_USAGE_FLAG_BITS_MAX_ENUM;
            }
        }

        std::tuple<bool, u32> FindMemoryType(Context::Ref context, u32 typeFilter, VkMemoryPropertyFlags properties)
        {
            VkPhysicalDeviceMemoryProperties memProperties;
            vkGetPhysicalDeviceMemoryProperties(context->physicalDevice, &memProperties);
            
            for (u32 i = 0; i < memProperties.memoryTypeCount; i++)
            {
                if (typeFilter & (1 << i) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties)
                {
                    return std::make_tuple(true, i);
                }
            }

            return std::make_tuple(false, 0);
        }
    }

    Buffer_Vulkan::Ref Buffer_Vulkan::Create(
          std::shared_ptr<Context> context
        , BufferType type
        , u64 blobSize
        , void const* blob)
    {
        auto result = std::make_shared<Buffer_Vulkan>(context);
        if (!result->InitialiseBuffer(context, type, blobSize))
        {
            return nullptr;
        }

        if (!result->InitialiseMemory(context, blob))
        {
            return nullptr;
        }

        result->Set(blobSize, blob);
        
        return result;
    }
    
    Buffer_Vulkan::Buffer_Vulkan(std::shared_ptr<Context> context)
        : context(context)
    {

    }

    Buffer_Vulkan::~Buffer_Vulkan()
    {
        if (buffer)
        {
            vkDestroyBuffer(context->device, buffer, nullptr);
            vkFreeMemory(context->device, bufferMemory, nullptr);
        }
    }
    
    void Buffer_Vulkan::Set(
          u64 blobSize
        , void const* blob)
    {
        if (blob == nullptr)
            return;

        void* mappedMemory = nullptr;
        vkMapMemory(context->device, bufferMemory, 0, blobSize, 0, &mappedMemory); {
            std::memcpy(mappedMemory, blob, blobSize);
        } vkUnmapMemory(context->device, bufferMemory);
    }

    bool Buffer_Vulkan::InitialiseBuffer(
          std::shared_ptr<Context> context
        , BufferType type
        , u64 blobSize)
    {
        VkBufferCreateInfo bufferInfo{};
        bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferInfo.size = blobSize;
        bufferInfo.usage = utils::FromBufferType(type);
        bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        if (vkCreateBuffer(context->device, &bufferInfo, nullptr, &buffer) != VK_SUCCESS)
        {
            LOG_ERROR("Failed to create buffer!\n");
            return false;
        }

        return true;
    }

    bool Buffer_Vulkan::InitialiseMemory(
          std::shared_ptr<Context> context
        , void const* blob)
    {
        VkMemoryRequirements memRequirements;
        vkGetBufferMemoryRequirements(context->device, buffer, &memRequirements);

        auto [foundMemoryType, memoryIndex] = utils::FindMemoryType(context
            , memRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
        if (!foundMemoryType)
        {
            LOG_ERROR("Failed to find suitable memory type!");
            return false;
        }

        VkMemoryAllocateInfo allocInfo = {};
        allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.allocationSize = memRequirements.size;
        allocInfo.memoryTypeIndex = memoryIndex;
        
        if (vkAllocateMemory(context->device, &allocInfo, nullptr, &bufferMemory) != VK_SUCCESS)
        {
            LOG_ERROR("Failed to allocate buffer memory!");
            return false;
        }

        if (vkBindBufferMemory(context->device, buffer, bufferMemory, 0) != VK_SUCCESS)
        {
            LOG_ERROR("Failed to bind buffer memory!");
            return false;
        }

        return true;
    }

    VertexArray::Ref VertexArray::Create(
          std::shared_ptr<Context> context
        , u64 vertexCount
        , u64 vertexStride
        , void const* blob)
    {
        auto result = std::make_shared<VertexArray>(vertexCount);

        u64 blobSize = vertexCount * vertexStride;
        if (!result->InitialiseBuffers(context, blobSize, blob))
        {
            LOG_ERROR("Failed to initialise buffers!");
            return nullptr;
        }

        return result;
    }

    VertexArray::VertexArray(u64 vertexCount)
        : vertexCount(vertexCount) {}

    bool VertexArray::InitialiseBuffers(
        std::shared_ptr<Context> context
        , u64 blobSize
        , void const* blob)
    {
        vertexBuffer = Buffer_Vulkan::Create(context, BufferType::VERTEX, blobSize, blob);
        return vertexBuffer != nullptr;
    }
}
#endif