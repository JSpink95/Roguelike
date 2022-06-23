#include "Graphics\Pipeline.h"

#include <numeric>

u64 PipelineSpecification::stride() const
{
    return (u64)std::accumulate(input.begin(), input.end(), 0u, [](u64 l, VertexAttributeType r) -> u64
    {
        return l + core::AttributeTypeToByteCount(r);
    });
}
