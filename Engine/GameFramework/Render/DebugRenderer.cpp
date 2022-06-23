#include "DebugRenderer.h"

// TEMP
#include <Graphics\OpenGL\CoreGL.h>

struct DebugRenderer::InternalData
{
    static constexpr u64 MaxLines = 1000u;
    static constexpr u64 MaxVertices = MaxLines * 2u;

    IShaderProgram::Ref shader = nullptr;
    IPipeline::Ref pipeline = nullptr;
    IVertexArray::Ref mesh = nullptr;

    DebugRenderer::Vertex* vertices = nullptr;
    DebugRenderer::Vertex* vertexIterator = nullptr;

    InternalData(IGraphicsDevice::Ref graphics)
    {
        shader = graphics->CreateShaderProgram({
            { "assets/shaders/compiled/debug.vert.spv", ShaderFlags::VERTEX },
            { "assets/shaders/compiled/debug.frag.spv", ShaderFlags::FRAGMENT }, });

        PipelineSpecification pipelineSpecification = {};
        pipelineSpecification.input = DebugRenderer::Vertex::Attributes;
        pipelineSpecification.shader = shader;
        pipelineSpecification.topologyMode = TopologyMode::LINES;
        pipelineSpecification.descriptors = { graphics->GetPerFrameShaderTransferBlock() };

        pipeline = graphics->CreatePipeline(pipelineSpecification);
        mesh = pipeline->CreateVertexArray(MaxVertices, nullptr);

        vertices = new DebugRenderer::Vertex[MaxVertices];
        vertexIterator = vertices;
    }

    ~InternalData()
    {
        delete[] vertices;
    }
};

DebugRenderer::Ref DebugRenderer::Create(IGraphicsDevice::Ref graphics)
{
    return std::make_shared<DebugRenderer>(graphics);
}

DebugRenderer::DebugRenderer(IGraphicsDevice::Ref graphics)
    : graphics(graphics)
    , internalData(std::make_unique<InternalData>(graphics))
{

}

void DebugRenderer::OpenVertexStream()
{
    internalData->vertexIterator = internalData->vertices;
}

void DebugRenderer::CloseVertexStream()
{
    glLineWidth(2.0f);
    u64 vertexCount = std::distance(internalData->vertices, internalData->vertexIterator);
    internalData->mesh->UpdateVertexBuffer(vertexCount, internalData->vertices);

    if (vertexCount > 0u)
    {
        internalData->pipeline->Submit(nullptr, internalData->mesh, vertexCount);
    }
    glLineWidth(1.0f);
}

void DebugRenderer::DrawBox2D(vec2 const& min, vec2 const& max, vec4 const& color)
{
    DrawLine2D(min, vec2(max.x, min.y), color);
    DrawLine2D(vec2(max.x, min.y), max, color);
    DrawLine2D(max, vec2(min.x, max.y), color);
    DrawLine2D(vec2(min.x, max.y), min, color);
}

void DebugRenderer::DrawCircle2D(vec2 const& centre, float radius, vec4 const& color)
{
    float angle = 0.0f;
    for (u32 index = 0; index < 12; ++index)
    {
        float x0 = glm::cos(glm::radians(angle));
        float y0 = glm::sin(glm::radians(angle));

        angle += 360.0f / 12.0f;
        float x1 = glm::cos(glm::radians(angle));
        float y1 = glm::sin(glm::radians(angle));

        DrawLine2D(vec2(x0, y0) * radius + centre, vec2(x1, y1) * radius + centre, color);
    }
}

void DebugRenderer::DrawLine2D(vec2 const& start, vec2 const& end, vec4 const& color)
{
    Vertex vertices[2] = {
        { vec3(start, 2.0f), color },
        { vec3(end, 2.0f), color },
    };

    SubmitVertices(vertices, 2);
}

void DebugRenderer::SubmitVertices(Vertex const* vertices, u32 count)
{
    for (u32 index = 0; index < count; ++index)
    {
        *internalData->vertexIterator = vertices[index];
        ++internalData->vertexIterator;
    }
}