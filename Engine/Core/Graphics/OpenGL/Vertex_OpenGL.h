#pragma once
#include "Core.h"
#include "Graphics\vertex.h"
#include "Graphics\OpenGL\CoreGL.h"

namespace gl
{
    struct Buffer : public IBuffer
    {
        BIND_OBJECT(Buffer);

        static Ref Create(
              BufferType type
            , u64 size
            , void const* data = nullptr);

        void Set(u64 blobSize, void const* blob);

        GLuint id;
        BufferType bufferType;
    };

    struct VertexArray: public IVertexArray
    {
        BIND_OBJECT(VertexArray);

        static Ref Create(
            std::vector<VertexAttributeType> const& attributes
            , u64 stride
            , u64 vertexCount
            , void const* vertices);

        static Ref Create(
              std::vector<VertexAttributeType> const& attributes
            , u64 stride
            , u64 indexCount
            , void const* indices
            , u64 vertexCount
            , void const* vertices);

        void UpdateVertexBuffer(
              u64 vertexCount
            , void const* vertexData) final;

        GLuint id = 0u;
        u64 indexCount = 0u;
        u64 vertexCount = 0u;

        Buffer::Ref indexBuffer = nullptr;
        Buffer::Ref vertexBuffer = nullptr;

    private:
        bool InitialiseVertexArray();
        bool InitialiseIndexBuffer(
              u64 indexCount
            , void const* indices);
        bool InitialiseVertexBuffer(
              std::vector<VertexAttributeType> const& attributes
            , u64 stride
            , u64 vertexCount
            , void const* vertices);

        u64 vertexStride = 0u;
    };
}