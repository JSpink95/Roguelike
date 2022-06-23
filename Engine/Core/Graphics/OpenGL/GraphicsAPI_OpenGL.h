#pragma once
#include "Core.h"
#include "Graphics\GraphicsAPI.h"
#include <Windows.h>

namespace gl
{
    struct GraphicsAPI : public IGraphicsDevice
    {
        BIND_OBJECT(GraphicsAPI);

        GraphicsAPI();
        ~GraphicsAPI();
        
        IShaderProgram::Ref CreateShaderProgram(
              std::string_view path
            , ShaderProgramConfig const& config) final;

        IShaderProgram::Ref CreateShaderProgram(
            std::vector<Shader> const& shaders) final;

        IShaderTransferBlock::Ref CreateShaderTransferBlock(
              u64 blobSize
            , void const* blob
            , u64 binding) final;

        // pipeline creation
        IPipeline::Ref CreatePipeline(
            PipelineSpecification const& spec) final;

        // render pass
        IRenderPass::Ref CreateRenderPass(
            RenderPassSpecification const& spec) final;

        IRenderPass::Ref GetDefaultRenderPass() final;

        // texture creation
        ITexture::Ref CreateTexture(
              int width
            , int height
            , int components
            , PrimitiveType type
            , void* pixels
            , TextureSpecification const& spec) final;

        ITexture::Ref CreateTexture(
              std::string_view path
            , TextureSpecification const& spec) final;

        ICommand::Ref BeginFrame() final;
        void FinishFrame() final;
    protected:
        void InitialiseInternal(std::shared_ptr<AppWindow> window) final;
    };
}