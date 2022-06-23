#include "Tile\TileGridRenderer.h"

struct TileGridRenderer::InternalData
{
    static constexpr u64 MaxQuadCount = 256u * 256u;
    static constexpr u64 MaxTriangleCount = MaxQuadCount * 2u;
    static constexpr u64 MaxVertexCount = MaxTriangleCount * 3u;

    IShaderProgram::Ref shader = nullptr;
    IPipeline::Ref pipeline = nullptr;

    TileGridRenderer::Vertex* vertexData = nullptr;
    TileGridRenderer::Vertex* vertexIterator = nullptr;
    IVertexArray::Ref mesh = nullptr;

    InternalData(IGraphicsDevice::Ref api, std::vector<TextureObject> const& textures)
    {
        shader = api->CreateShaderProgram({
            { "assets/shaders/compiled/sprite_batch.vert.spv", ShaderFlags::VERTEX },
            { "assets/shaders/compiled/sprite_batch.frag.spv", ShaderFlags::FRAGMENT }, });

        PipelineSpecification pipelineSpecification = {};
        pipelineSpecification.input = TileGridRenderer::Vertex::Attributes;
        pipelineSpecification.topologyMode = TopologyMode::TRIANGLES;
        pipelineSpecification.shader = shader;
        pipelineSpecification.textures = textures;
        pipelineSpecification.descriptors = {
            api->GetPerFrameShaderTransferBlock(),
            api->GetPerObjectShaderTransferBlock() };

        pipeline = api->CreatePipeline(pipelineSpecification);
        mesh = pipeline->CreateVertexArray(MaxVertexCount, nullptr);

        vertexData = new TileGridRenderer::Vertex[MaxVertexCount];
        vertexIterator = vertexData;
    }

    ~InternalData()
    {
        delete[] vertexData;
    }
};

TileGridRenderer::Ref TileGridRenderer::Create(IGraphicsDevice::Ref api, TileGridRendererSpecification const& spec)
{
    return std::make_shared<TileGridRenderer>(api, spec);
}

TileGridRenderer::TileGridRenderer(IGraphicsDevice::Ref api, TileGridRendererSpecification const& spec)
    : graphicsApi(api)
    , specification(spec)
    , internalData(std::make_unique<InternalData>(graphicsApi, spec.textures))
{

}

void TileGridRenderer::OpenVertexStream()
{
    // reset the iterator
    internalData->vertexIterator = internalData->vertexData;
}

void TileGridRenderer::CloseVertexStream()
{
    u64 vertexCount = std::distance(internalData->vertexData, internalData->vertexIterator);
    internalData->mesh->UpdateVertexBuffer(vertexCount, internalData->vertexData);

    if (vertexCount > 0u)
    {
        // TODO(jack): get render command
        internalData->pipeline->Submit(nullptr, internalData->mesh, vertexCount);
    }
}

void TileGridRenderer::SubmitTile(float x, float y, int spriteIndex, int textureIndex)
{
    SubmitTile(x, y, 1.0f, spriteIndex, textureIndex);
}

void TileGridRenderer::SubmitTile(float x, float y, float size, int spriteIndex, int textureIndex)
{
    float depth = 0.0f; // TODO(jack): get depth from layer
    float tileUSize = (1.0f / (float)specification.imageSizePixels.x);
    float tileVSize = (1.0f / (float)specification.imageSizePixels.y);

    float tileAspect = tileVSize / tileUSize;

    float tileSheetSingleWidth = tileUSize * (float)specification.spriteSizePixels.x;
    float tileSheetSingleHeight = tileVSize * (float)specification.spriteSizePixels.y;

    int spriteX = spriteIndex % specification.spriteSizePixels.x;
    int spriteY = spriteIndex / specification.spriteSizePixels.x;

    vec2 uvs[4] = {
        vec2(spriteX * tileSheetSingleWidth                       , spriteY * tileSheetSingleHeight + tileSheetSingleHeight),
        vec2(spriteX * tileSheetSingleWidth + tileSheetSingleWidth, spriteY * tileSheetSingleHeight + tileSheetSingleHeight),
        vec2(spriteX * tileSheetSingleWidth + tileSheetSingleWidth, spriteY * tileSheetSingleHeight),
        vec2(spriteX * tileSheetSingleWidth                       , spriteY * tileSheetSingleHeight),
    };

    Vertex vertices[6] = {
        { vec3(x + 0.0f, y + 0.0f, depth), uvs[0], (float)textureIndex },
        { vec3(x + size, y + 0.0f, depth), uvs[1], (float)textureIndex },
        { vec3(x + size, y + size * tileAspect, depth), uvs[2], (float)textureIndex },

        { vec3(x + 0.0f, y + 0.0f, depth), uvs[0], (float)textureIndex },
        { vec3(x + size, y + size * tileAspect, depth), uvs[2], (float)textureIndex },
        { vec3(x + 0.0f, y + size * tileAspect, depth), uvs[3], (float)textureIndex },
    };

    SubmitVertexStream(vertices, 6);
}

void TileGridRenderer::SubmitVertexStream(Vertex* vertices, u64 count)
{
    for (u64 index = 0; index < count; ++index)
    {
        *internalData->vertexIterator = vertices[index];
        ++internalData->vertexIterator;
    }
}
