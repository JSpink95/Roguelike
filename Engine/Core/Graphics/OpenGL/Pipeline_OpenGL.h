#pragma once
#include "Core.h"
#include "Graphics\Pipeline.h"

namespace gl
{
    struct GraphicsAPI;

    struct Pipeline : public IPipeline
    {
        BIND_OBJECT(Pipeline);

        static Ref Create(
            std::shared_ptr<GraphicsAPI> graphics
            , PipelineSpecification const& spec);

        Pipeline(PipelineSpecification const& spec);

        void Submit(
              ICommand::Ref command
            , std::shared_ptr<IVertexArray> stream
            , u64 vertexCount = 0) final;
        
        std::shared_ptr<IVertexArray> CreateVertexArray(
              u64 vertexCount
            , void const* vertices) final;

        PipelineSpecification specification;
    };
}