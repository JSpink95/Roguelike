#include "Graphics\OpenGL\Vertex_OpenGL.h"
#include "Graphics\Material.h"

namespace gl
{
    namespace utils
    {
        GLenum FromBufferType(BufferType type)
        {
            switch (type)
            {
                case BufferType::INDEX:
                    return GL_ELEMENT_ARRAY_BUFFER;
                case BufferType::VERTEX:
                    return GL_ARRAY_BUFFER;
                default:
                    return GL_NONE;
            }
        }

        bool IsIntType(VertexAttributeType type)
        {
            switch (type)
            {
                case VertexAttributeType::INT:
                case VertexAttributeType::INT2:
                case VertexAttributeType::INT3:
                case VertexAttributeType::INT4:
                    return true;
                default:
                    return false;
            }
        }

        u64 ElementCountFromAttributeType(VertexAttributeType type)
        {
            switch (type)
            {
                case VertexAttributeType::INT:
                case VertexAttributeType::FLOAT:
                    return 1;

                case VertexAttributeType::INT2:
                case VertexAttributeType::FLOAT2:
                    return 2;

                case VertexAttributeType::INT3:
                case VertexAttributeType::FLOAT3:
                    return 3;

                case VertexAttributeType::INT4:
                case VertexAttributeType::FLOAT4:
                    return 4;

                case VertexAttributeType::MAT4:
                    return 16;

                default:
                    return 0;
            }
        }
    }

    Buffer::Ref Buffer::Create(
          BufferType type
        , u64 size
        , void const* data/* = nullptr*/)
    {
        auto result = std::make_shared<Buffer>();
        result->bufferType = type;

        glGenBuffers(1, &result->id);
        glBindBuffer(utils::FromBufferType(type), result->id);
        glBufferData(utils::FromBufferType(type), size, data, GL_STATIC_DRAW);
        glBindBuffer(utils::FromBufferType(type), 0);

        return result;
    }

    void Buffer::Set(u64 blobSize, void const* blob)
    {
        glBindBuffer(utils::FromBufferType(bufferType), id);
        glBufferData(utils::FromBufferType(bufferType), blobSize, blob, GL_STATIC_DRAW);
        glBindBuffer(utils::FromBufferType(bufferType), 0);
    }

    VertexArray::Ref VertexArray::Create(
          std::vector<VertexAttributeType> const& attributes
        , u64 stride
        , u64 vertexCount
        , void const* vertices)
    {
        auto result = std::make_shared<VertexArray>();
        if (!result->InitialiseVertexArray())
        {
            return nullptr;
        }

        if (!result->InitialiseVertexBuffer(attributes, stride, vertexCount, vertices))
        {
            return nullptr;
        }

        return result;
    }

    VertexArray::Ref VertexArray::Create(
          std::vector<VertexAttributeType> const& attributes
        , u64 stride
        , u64 indexCount
        , void const* indices
        , u64 vertexCount
        , void const* vertices)
    {
        auto result = std::make_shared<VertexArray>();
        if (!result->InitialiseVertexArray())
        {
            return nullptr;
        }

        if (!result->InitialiseIndexBuffer(indexCount, vertices))
        {
            return nullptr;
        }

        if (!result->InitialiseVertexBuffer(attributes, stride, vertexCount, vertices))
        {
            return nullptr;
        }

        return result;
    }
    
    void VertexArray::UpdateVertexBuffer(
          u64 vertexCount
        , void const* vertexData)
    {
        vertexBuffer->Set(vertexCount * vertexStride, vertexData);
    }

    bool VertexArray::InitialiseVertexArray()
    {
        //glGenVertexArrays(1, &id);
        glCreateVertexArrays(1, &id);
        return true;
    }

    bool VertexArray::InitialiseIndexBuffer(u64 indexCount, void const* indices)
    {
        indexBuffer = Buffer::Create(BufferType::INDEX, indexCount * sizeof(u32), indices);
        this->indexCount = indexCount;
        return indexBuffer != nullptr;
    }

    bool VertexArray::InitialiseVertexBuffer(
          std::vector<VertexAttributeType> const& attributes
        , u64 stride
        , u64 vertexCount
        , void const* vertices)
    {
        vertexStride = stride;

        glBindVertexArray(id);

        u64 blobSize = stride * vertexCount;
        vertexBuffer = Buffer::Create(BufferType::VERTEX, blobSize, vertices);
        //glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer->id);

        u64 attributeOffset = 0u;
        for (u32 attributeIndex = 0; attributeIndex < attributes.size(); ++attributeIndex)
        {
            auto attribute = attributes.at(attributeIndex);

            glEnableVertexAttribArray(attributeIndex);

            if (utils::IsIntType(attribute))
            {
                glVertexAttribIFormat(
                      attributeIndex
                    , utils::ElementCountFromAttributeType(attribute)
                    , GL_INT // TODO(jack): again, conversion function
                    , attributeOffset);
            }
            else
            {
                glVertexAttribFormat(
                      attributeIndex
                    , utils::ElementCountFromAttributeType(attribute)
                    , GL_FLOAT // TODO(jack): add conversion function 
                    , GL_FALSE
                    , attributeOffset);
            }

            
            glVertexAttribBinding(attributeIndex, 0);
            glBindVertexBuffer(attributeIndex, vertexBuffer->id, 0u, (GLsizei)stride);

            attributeOffset += core::AttributeTypeToByteCount(attribute);
        }

        this->vertexCount = vertexCount;
        glBindVertexArray(0);

        return vertexBuffer != nullptr;
    }
}