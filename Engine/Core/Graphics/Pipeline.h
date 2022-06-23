#pragma once
#include "Core.h"
#include "Core\Object.h"
#include "Graphics\Types.h"
#include "Graphics\Command.h"
#include "Graphics\ShaderProgram.h"
#include "Graphics\RenderPass.h"
#include "Graphics\Texture.h"

#include <vector>

struct IVertexArray;

enum class TopologyMode
{
    POINTS, LINES, TRIANGLES,
};

struct PipelineSpecification
{
    IShaderProgram::Ref shader = nullptr;
    IRenderPass::Ref renderPass = nullptr;
    std::vector<VertexAttributeType> input = {};
    TopologyMode topologyMode = TopologyMode::LINES;
    std::vector<IShaderTransferBlock::Ref> descriptors = {};
    std::vector<TextureObject> textures = {};

    PipelineSpecification() = default;

    u64 stride() const;
};

struct IPipeline: public Object
{
    BIND_OBJECT(IPipeline);

    virtual void Submit(
          ICommand::Ref command
        , std::shared_ptr<IVertexArray> stream
        , u64 vertexCount = 0) = 0;

    virtual std::shared_ptr<IVertexArray> CreateVertexArray(
          u64 vertexCount
        , void const* vertices) = 0;
};