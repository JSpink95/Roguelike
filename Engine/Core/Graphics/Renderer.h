#pragma once
#include "Core.h"
#include "Graphics\ShaderProgram.h"

#include <array>

#define SHADER_TRANSFER_BLOCK_STRUCT(name) __declspec(align(16)) struct name

struct IGraphicsDevice;

constexpr u64 MAX_SHDER_TRASNSFER_BLOCKS = 8u;

namespace ShaderTransferBlockIndex
{
    constexpr u64 PER_FRAME_MATRICES = 0u;
    constexpr u64 PER_OBJECT_MATRICES = 1u;
}

struct PerFrameMatrixShaderBlock
{
    alignas(16) glm::mat4 _ProjectionView;
};

struct PerObjectMatrixShaderBlock
{
    alignas(16) glm::mat4 _Transform;
};

struct RendererData
{
    std::array<std::shared_ptr<IShaderTransferBlock>, MAX_SHDER_TRASNSFER_BLOCKS> transferBlocks;

    RendererData(std::shared_ptr<IGraphicsDevice> api);
};

struct IRenderer
{
    std::shared_ptr<RendererData> data;

    IRenderer(std::shared_ptr<RendererData> rd)
        : data(rd) {}

    virtual ~IRenderer() = default;
};