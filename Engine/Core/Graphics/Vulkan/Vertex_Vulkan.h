#pragma once
#include "Core.h"

#ifdef _VULKAN
#include "Graphics\Vertex.h"
#include "Graphics\Vulkan\CoreVK.h"

namespace vk
{
    struct Context;

    struct Buffer_Vulkan : public IBuffer
    {
        BIND_OBJECT(Buffer_Vulkan);
        static Ref Create(
              std::shared_ptr<Context> context
            , BufferType type
            , u64 blobSize
            , void const* blob);

        Buffer_Vulkan(std::shared_ptr<Context> context);
        virtual ~Buffer_Vulkan();

        void Set(
              u64 blobSize
            , void const* blob);

        std::shared_ptr<Context> context;
        VkBuffer buffer;
        VkDeviceMemory bufferMemory;

    private:
        bool InitialiseBuffer(
              std::shared_ptr<Context> context
            , BufferType type
            , u64 blobSize);

        bool InitialiseMemory(
              std::shared_ptr<Context> context
            , void const* blob);
    };

    struct VertexArray: public IVertexArray
    {
        BIND_OBJECT(VertexArray);

        static Ref Create(
            std::shared_ptr<Context> context
            , u64 vertexCount
            , u64 vertexStride
            , void const* blob);

        VertexArray(u64 vertexCount);

        u64 vertexCount = 0u;
        Buffer_Vulkan::Ref vertexBuffer;
    private:
        bool InitialiseBuffers(
              std::shared_ptr<Context> context
            , u64 blobSize
            , void const* blob);
    };
}
#endif