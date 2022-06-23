#include <Core.h>
#include <Core\Window.h>
#include <Graphics\GraphicsAPI.h>
#include <Render\QuadBatchRenderer.h>

#include <array>
#include <random>

struct FallingSandRenderer: public Object
{
    BIND_OBJECT(FallingSandRenderer);

    struct Vertex
    {
        static inline VertexAttributeTypeList Attributes = {
            VertexAttributeType::FLOAT2,
            VertexAttributeType::FLOAT4,
        };

        vec2 a_Position;
        vec4 a_Color;
    };

    static inline Ref Create(IGraphicsDevice::Ref graphicsDevice)
    {
        return std::make_shared<FallingSandRenderer>(graphicsDevice);
    }

    FallingSandRenderer(IGraphicsDevice::Ref graphicsDevice);

    void Begin();
    void Finish();

    void DrawDebugPixel(ivec2 const& pixel);
    void DrawSand(ivec2 const& pixel);
    void DrawPixel(ivec2 const& pixel, vec4 const& color);

    IGraphicsDevice::Ref graphicsDevice = nullptr;
    IPipeline::Ref pipeline = nullptr;
    VertexBatchRenderer<Vertex>::Ref batcher = nullptr;
};

FallingSandRenderer::FallingSandRenderer(IGraphicsDevice::Ref graphicsDevice)
    : graphicsDevice(graphicsDevice)
{
    PipelineSpecification pipelineSpec = {};
    pipelineSpec.shader = graphicsDevice->CreateShaderProgram({ 
        { "assets/shaders/compiled/falling_sand.vert.spv", ShaderFlags::VERTEX },
        { "assets/shaders/compiled/falling_sand.frag.spv", ShaderFlags::FRAGMENT }, });
    pipelineSpec.input = Vertex::Attributes;
    pipelineSpec.descriptors = { graphicsDevice->GetPerFrameShaderTransferBlock() };
    pipelineSpec.topologyMode = TopologyMode::POINTS;

    pipeline = graphicsDevice->CreatePipeline(pipelineSpec);
    batcher = VertexBatchRenderer<Vertex>::Create(pipeline);
}

void FallingSandRenderer::Begin()
{
    PerFrameMatrixShaderBlock perFrameMatrices = { glm::ortho(0.0f, 320.0f, 0.0f, 180.0f, -1.0f, 1.0f) };
    graphicsDevice->GetPerFrameShaderTransferBlock()->Set(sizeof(PerFrameMatrixShaderBlock), &perFrameMatrices);

    batcher->OpenVertexStream();
}

void FallingSandRenderer::Finish()
{
    auto vertexCount = batcher->CloseVertexStream();

    if (vertexCount > 0u)
    {
        pipeline->Submit(nullptr, batcher->GetMesh(), vertexCount);
    }
}

void FallingSandRenderer::DrawDebugPixel(ivec2 const& pixel)
{
    constexpr vec4 debugPixelColor = vec4(1.0f, 0.0f, 1.0f, 1.0f);
    DrawPixel(pixel, debugPixelColor);
}

void FallingSandRenderer::DrawSand(ivec2 const& pixel)
{
    constexpr vec4 sandPixelColor = vec4(0.92f, 0.88f, 0.05f, 1.0f);
    DrawPixel(pixel, sandPixelColor);
}

void FallingSandRenderer::DrawPixel(ivec2 const& pixel, vec4 const& color)
{
    Vertex vertex[1] = {
        { vec2(pixel) + 0.5f, color }
    };

    batcher->SubmitVertices(vertex, 1);
}

namespace FallingSand
{
    enum Type
    {
        Empty, Sand, Water,
        Max, OutOfBounds
    };
}

struct FallingSandParticle
{
    static FallingSandParticle OUT_OF_BOUNDS;
    FallingSand::Type type = FallingSand::Empty;

    // what direction to move in (used by water)
    int directionToMove = -1;
};

FallingSandParticle FallingSandParticle::OUT_OF_BOUNDS = { FallingSand::OutOfBounds };

struct FallingSandSimulator : public Object
{
    BIND_OBJECT(FallingSandSimulator);

    static constexpr u64 WIDTH = 320u;
    static constexpr u64 HEIGHT = 180u;

    static inline Ref Create()
    {
        return std::make_shared<FallingSandSimulator>();
    }

    FallingSandParticle const& Get(int x, int y) const;
    FallingSandParticle& Get(int x, int y);
    void Set(int x, int y, FallingSand::Type type, int directionToMove = 0);

    void Update();
    void UpdateSand(int x, int y);
    void UpdateWater(int x, int y);

    std::array<FallingSandParticle, WIDTH * HEIGHT> pixels = {};
};

FallingSandParticle& FallingSandSimulator::Get(int x, int y)
{
    if (x < 0 || x >= WIDTH || y < 0 || y >= HEIGHT)
        return FallingSandParticle::OUT_OF_BOUNDS;

    u64 index = (u64)x + (u64)y * WIDTH;
    return pixels.at(index);
}

FallingSandParticle const& FallingSandSimulator::Get(int x, int y) const
{
    if (x < 0 || x >= WIDTH || y < 0 || y >= HEIGHT)
        return FallingSandParticle::OUT_OF_BOUNDS;

    u64 index = (u64)x + (u64)y * WIDTH;
    return pixels.at(index);
}

void FallingSandSimulator::Set(int x, int y, FallingSand::Type type, int directionToMove/* = 0*/)
{
    if (x < 0 || x >= WIDTH || y < 0 || y >= HEIGHT)
        return;

    u64 index = (u64)x + (u64)y * WIDTH;
    pixels.at(index).type = type;
    pixels.at(index).directionToMove = directionToMove;
}

void FallingSandSimulator::Update()
{
    // tick sand
    // go from bottom to top, left to right
    for (u32 y = 0; y < HEIGHT; ++y)
    {
        for (u32 x = 0; x < WIDTH; ++x)
        {
            FallingSand::Type type = Get(x, y).type;
            if (type == FallingSand::Sand)
            {
                UpdateSand(x, y);
            }
        }
    }

    // tick water, 4 times per frame
    // go from bottom to top, left to right
    for (u32 waterTick = 0u; waterTick < 4; ++waterTick)
    {
        for (u32 y = 0; y < HEIGHT; ++y)
        {
            for (u32 x = 0; x < WIDTH; ++x)
            {
                FallingSand::Type type = Get(x, y).type;
                if (type == FallingSand::Water)
                {
                    UpdateWater(x, y);
                }
            }
        }
    }
}

void FallingSandSimulator::UpdateWater(int x, int y)
{
    auto& particle = Get(x, y);
    auto typeBelow = Get(x, y - 1).type;

    // if empty, drop down
    if (typeBelow == FallingSand::Empty)
    {
        Set(x, y, FallingSand::Empty);
        Set(x, y - 1, FallingSand::Water, particle.directionToMove);
        return;
    }

    auto typeNext = Get(x + particle.directionToMove, y).type;
    if (typeNext == FallingSand::Empty)
    {
        Set(x, y, FallingSand::Empty);
        Set(x + particle.directionToMove, y, FallingSand::Water, particle.directionToMove);
        return;
    }

    particle.directionToMove *= -1;
    typeNext = Get(x + particle.directionToMove, y).type;
    if (typeNext == FallingSand::Empty)
    {
        Set(x, y, FallingSand::Empty);
        Set(x + particle.directionToMove, y, FallingSand::Water, particle.directionToMove);
        return;
    }
    particle.directionToMove *= -1;
}

void FallingSandSimulator::UpdateSand(int x, int y)
{
    auto typeBelow = Get(x, y - 1).type;
    if (typeBelow == FallingSand::OutOfBounds)
        return;

    // if empty, drop down
    if (typeBelow == FallingSand::Empty)
    {
        Set(x, y, FallingSand::Empty);
        Set(x, y - 1, FallingSand::Sand);
        return;
    }

    // if water, swap position
    if (typeBelow == FallingSand::Water)
    {
        Set(x, y, FallingSand::Water, Get(x, y - 1).directionToMove);
        Set(x, y - 1, FallingSand::Sand);
        return;
    }


    // fall down in triangular form
    auto typeBelowLeft = Get(x - 1, y - 1).type;

    if (typeBelowLeft == FallingSand::Empty)
    {
        Set(x, y, FallingSand::Empty);
        Set(x - 1, y - 1, FallingSand::Sand);
        return;
    }

    if (typeBelowLeft == FallingSand::Water)
    {
        if (Get(x - 1, y).type == FallingSand::Empty)
        {
            Set(x - 1, y, FallingSand::Water, Get(x - 1, y).directionToMove);
            Set(x - 1, y - 1, FallingSand::Sand);
            return;
        }
    }

    auto typeBelowRight = Get(x + 1, y - 1).type;
    if (typeBelowRight == FallingSand::Empty)
    {
        Set(x, y, FallingSand::Empty);
        Set(x + 1, y - 1, FallingSand::Sand);
        return;
    }

    if (typeBelowRight == FallingSand::Water)
    {
        if (Get(x + 1, y).type == FallingSand::Empty)
        {
            Set(x + 1, y, FallingSand::Water, Get(x + 1, y).directionToMove);
            Set(x + 1, y - 1, FallingSand::Sand);
            return;
        }
    }
}

int main()
{
    AppWindowSpecification windowSpec = {};
    windowSpec.width = 1920;
    windowSpec.height = 1080;
    windowSpec.title = "Falling Sand";

    auto window = AppWindow::Create(windowSpec);

    RenderPassSpecification defaultRenderPassSpec = {};
    defaultRenderPassSpec.clearColor = vec4(0.0f, 0.0f, 0.0f, 0.0f);
    defaultRenderPassSpec.size = window->GetDimensions();
    defaultRenderPassSpec.attachments = {
        { RenderPassSpecification::Attachment::Format::RGBA, RenderPassSpecification::Attachment::Type::COLOR, 1u },
    };

    auto defaultRenderPass = window->graphics->CreateRenderPass(defaultRenderPassSpec);

    auto fallingSandSimulator = FallingSandSimulator::Create();
    auto fallingSandRenderer = FallingSandRenderer::Create(window->graphics);

    std::default_random_engine rengine;
    std::uniform_int_distribution<int> distributorX(0, 320);
    std::uniform_int_distribution<int> distributorY(32, 180);

    for (u32 sand = 0; sand < 800; ++sand) {
        int x = distributorX(rengine);
        int y = distributorY(rengine);

        fallingSandSimulator->Set(x, y, FallingSand::Sand);
    }

    for (u32 x = 0; x < FallingSandSimulator::WIDTH; ++x) {
        for (u32 y = 0; y < 2; ++y) {
            fallingSandSimulator->Set(x, y, FallingSand::Water);
        }
    }

    glPointSize(1920.0f / 320.0f);

    constexpr vec4 PixelColors[(u64)FallingSand::Max] =
    {
        vec4(0.0f), // empty
        vec4(0.92f, 0.88f, 0.05f, 1.0f), // sand
        vec4(0.05f, 0.32f, 0.99f, 1.0f), // water
    };

    while (!window->closeRequested)
    {
        fallingSandSimulator->Update();

        defaultRenderPass->Begin(nullptr);

        fallingSandRenderer->Begin();
        {
            for (u32 x = 0; x < FallingSandSimulator::WIDTH; ++x)
            {
                for (u32 y = 0; y < FallingSandSimulator::HEIGHT; ++y)
                {
                    auto type = fallingSandSimulator->Get(x, y).type;
                    if (type != FallingSand::Empty && type != FallingSand::OutOfBounds)
                    {
                        fallingSandRenderer->DrawPixel(ivec2(x, y), PixelColors[(u32)type]);
                    }
                }
            }
        }
        fallingSandRenderer->Finish();

        defaultRenderPass->End();

        window->PollEvents();
        window->Present();
    }

    return 0;
}