#include "Render\SpriteRenderer.h"
#include "Game\Components.h"

#include <algorithm>
#include <iterator>

SpriteRenderer::Ref SpriteRenderer::Create(IGraphicsDevice::Ref api, SpriteRendererSpecification const& spec)
{
    return std::make_shared<SpriteRenderer>(api, spec);
}

SpriteRenderer::SpriteRenderer(IGraphicsDevice::Ref api, SpriteRendererSpecification const& spec)
    : graphicsDevice(api)
    , specification(spec)
{
    auto shader = api->CreateShaderProgram({
        { "assets/shaders/compiled/sprite_batch.vert.spv", ShaderFlags::VERTEX },
        { "assets/shaders/compiled/sprite_batch.frag.spv", ShaderFlags::FRAGMENT }, });
    
    PipelineSpecification pipelineSpecification = {};
    pipelineSpecification.input = SpriteRenderer::Vertex::Attributes;
    pipelineSpecification.topologyMode = TopologyMode::TRIANGLES;
    pipelineSpecification.shader = shader;
    pipelineSpecification.textures = spec.textures;

    std::transform(spec.textures.begin(), spec.textures.end(), std::back_inserter(spriteSheets), [this](TextureObject const& texture) -> SpriteSheet::Ref
    {
        SpriteSheetSpecification specification = {};
        specification.spriteSheetSizePixels = this->specification.textureSize;
        specification.spriteSizePixels = this->specification.spriteSize;
        specification.texture = texture.texture;

        return SpriteSheet::Create(specification);
    });
    
    pipelineSpecification.descriptors = {
        api->GetPerFrameShaderTransferBlock(),
        api->GetPerObjectShaderTransferBlock() };

    pipeline = api->CreatePipeline(pipelineSpecification);
    batcher = VertexBatchRenderer<Vertex>::Create(pipeline);
}

void SpriteRenderer::SetWorldTransform(glm::mat4 const& transform)
{
    PerObjectMatrixShaderBlock perObjectMatrices = { transform };
    graphicsDevice->GetPerObjectShaderTransferBlock()->Set(sizeof(PerObjectMatrixShaderBlock), &perObjectMatrices);
}

void SpriteRenderer::Begin(Entity::Ref camera)
{
    if (camera->HasComponent<CameraComponent>())
    {
        auto& cameraComponent = camera->GetComponent<CameraComponent>();
        PerFrameMatrixShaderBlock perFrameMatrices = { cameraComponent.GetProjectionView(camera) };
        graphicsDevice->GetPerFrameShaderTransferBlock()->Set(sizeof(PerFrameMatrixShaderBlock), &perFrameMatrices);
    }

    // reset the iterator
    batcher->OpenVertexStream();
}

void SpriteRenderer::Finish()
{
    u64 vertexCount = batcher->CloseVertexStream();

    if (vertexCount > 0u)
    {
        // TODO(jack): get render command
        pipeline->Submit(nullptr, batcher->GetMesh(), vertexCount);
    }
}

void SpriteRenderer::SubmitSprite(float x, float y, float z, float size, Sprite::Ref sprite, float alpha/* = 1.0f*/)
{
    if (sprite)
    {
        SubmitSprite(x, y, z, vec2(size), sprite, alpha);
    }
}

void SpriteRenderer::SubmitSprite(float x, float y, float z, vec2 const& size, Sprite::Ref sprite, float alpha/* = 1.0f*/)
{
    if (sprite)
    {
        u32 spriteLayerIndex = (u32)std::distance(spriteSheets.begin(), std::find(spriteSheets.begin(), spriteSheets.end(), sprite->spriteSheet));
        SubmitSprite(x, y, z, size, sprite->spriteIndex, spriteLayerIndex, alpha);
    }
}

void SpriteRenderer::SubmitSprite(float x, float y, float z, vec2 const& size, u32 spriteIndex, u32 spriteLayer, float alpha/* = 1.0f*/)
{
    SpriteSheet::Ref spriteSheet = spriteSheets.at(spriteLayer);
    auto uvBounds = spriteSheet->GetUVBounds(spriteIndex);
    if (specification.flipTextureCoords)
    {
        std::swap(uvBounds.y, uvBounds.w);
    }

    Vertex vertices[6] =
    {
        { vec3(x + 0.0f  , y + 0.0f  , z), vec2(uvBounds.x, uvBounds.y), (float)spriteLayer, alpha },
        { vec3(x + size.x, y + 0.0f  , z), vec2(uvBounds.z, uvBounds.y), (float)spriteLayer, alpha },
        { vec3(x + size.x, y + size.y, z), vec2(uvBounds.z, uvBounds.w), (float)spriteLayer, alpha },

        { vec3(x + 0.0f  , y + 0.0f  , z), vec2(uvBounds.x, uvBounds.y), (float)spriteLayer, alpha },
        { vec3(x + size.x, y + size.y, z), vec2(uvBounds.z, uvBounds.w), (float)spriteLayer, alpha },
        { vec3(x + 0.0f  , y + size.y, z), vec2(uvBounds.x, uvBounds.w), (float)spriteLayer, alpha },
    };

    batcher->SubmitVertices(vertices, 6);
}

Sprite::Ref SpriteRenderer::CreateSprite(u32 layer, u32 index) const
{
    auto spriteSheet = GetSpriteSheet(layer);
    if (spriteSheet == nullptr)
        return nullptr;

    return Sprite::Create(spriteSheet, index);
}

SpriteSheet::Ref SpriteRenderer::GetSpriteSheet(u32 layer) const
{
    if (layer >= spriteSheets.size())
        return nullptr;

    return spriteSheets.at(layer);
}