template<typename TVertex>
struct VertexBatchRenderer<TVertex>::InternalData
{
    static constexpr u32 MaxQuads = 10000u;
    static constexpr u32 MaxTriangles = MaxQuads * 2;
    static constexpr u32 MaxVertices = MaxTriangles * 3;

    InternalData(IPipeline::Ref pipeline);
    ~InternalData();

    IVertexArray::Ref mesh = nullptr;
    TVertex* vertices = nullptr;
    TVertex* vertexIterator = nullptr;
};

template<typename TVertex> 
VertexBatchRenderer<TVertex>::InternalData::InternalData(IPipeline::Ref pipeline)
{
    mesh = pipeline->CreateVertexArray(MaxVertices, nullptr);

    vertices = new TVertex[MaxVertices];
    vertexIterator = vertices;
}
template<typename TVertex>
VertexBatchRenderer<TVertex>::InternalData::~InternalData()
{
    delete[] vertices;
    vertices = nullptr;
    vertexIterator = nullptr;
}
template<typename TVertex>
typename VertexBatchRenderer<TVertex>::Ref VertexBatchRenderer<TVertex>::Create(IPipeline::Ref pipeline)
{
    return std::make_shared<VertexBatchRenderer<TVertex>>(pipeline);
}
template<typename TVertex>
VertexBatchRenderer<TVertex>::VertexBatchRenderer(IPipeline::Ref pipeline)
    : internalData(std::make_unique<InternalData>(pipeline))
{

}
template<typename TVertex>
void VertexBatchRenderer<TVertex>::OpenVertexStream()
{
    internalData->vertexIterator = internalData->vertices;
}
template<typename TVertex>
u32 VertexBatchRenderer<TVertex>::CloseVertexStream()
{
    u32 vertexCount = (u32)std::distance(internalData->vertices, internalData->vertexIterator);
    internalData->mesh->UpdateVertexBuffer(vertexCount, internalData->vertices);

    return vertexCount;
}
template<typename TVertex>
void VertexBatchRenderer<TVertex>::SubmitVertices(TVertex* vertices, u32 count)
{
    for (u32 index = 0u; index < count; ++index)
    {
        *internalData->vertexIterator = vertices[index];
        ++internalData->vertexIterator;
    }
}
template<typename TVertex>
inline IVertexArray::Ref VertexBatchRenderer<TVertex>::GetMesh()
{
    return internalData->mesh;
}