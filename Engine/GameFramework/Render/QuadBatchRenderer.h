#pragma once
#include "GameCore.h"
#include <Core\Object.h>
#include <Graphics\GraphicsAPI.h>

template<typename TVertex>
struct VertexBatchRenderer: public Object
{
    struct InternalData;
    TBIND_OBJECT(VertexBatchRenderer, TVertex);

    static Ref Create(IPipeline::Ref pipeline);
    VertexBatchRenderer(IPipeline::Ref pipeline);

    void OpenVertexStream();
    u32 CloseVertexStream();

    void SubmitVertices(TVertex* vertices, u32 count);

    IVertexArray::Ref GetMesh();

    std::unique_ptr<InternalData> internalData;
};
#include "QuadBatchRenderer.inl"