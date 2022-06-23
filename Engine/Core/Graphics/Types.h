#pragma once
#include "Core.h"
#include <vector>

// frame clear flags
BITFIELD_BEGIN(ClearFlags)
    BITFIELD_VALUE(COLOR, 0);
    BITFIELD_VALUE(DEPTH, 1);
BITFIELD_END()

enum class PrimitiveType
{
    INT8, INT16, INT32,
    UINT8, UINT16, UINT32,
    FLOAT,
};

enum class BufferType : u8
{
    INDEX, VERTEX, UNIFORM
};

enum class VertexAttributeType: u8
{
    INT, INT2, INT3, INT4,
    FLOAT, FLOAT2, FLOAT3, FLOAT4,
    MAT4
};

using VertexAttributeTypeList = std::vector<VertexAttributeType>;

namespace core
{
    inline u64 AttributeTypeToByteCount(VertexAttributeType type)
    {
        switch (type)
        {
            case VertexAttributeType::INT:
                return sizeof(int) * 1;
            case VertexAttributeType::INT2:
                return sizeof(int) * 2;
            case VertexAttributeType::INT3:
                return sizeof(int) * 3;
            case VertexAttributeType::INT4:
                return sizeof(int) * 4;
            case VertexAttributeType::FLOAT:
                return sizeof(float) * 1;
            case VertexAttributeType::FLOAT2:
                return sizeof(float) * 2;
            case VertexAttributeType::FLOAT3:
                return sizeof(float) * 3;
            case VertexAttributeType::FLOAT4:
                return sizeof(float) * 4;
            case VertexAttributeType::MAT4:
                return sizeof(float) * 4 * 4;
            default:
                return 0u;
        }
    }
}
