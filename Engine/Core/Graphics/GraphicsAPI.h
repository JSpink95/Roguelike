#pragma once
#include "Core.h"
#include "Core\Object.h"

#include "Graphics\Types.h"
#include "Graphics\Texture.h"
#include "Graphics\Framebuffer.h"
#include "Graphics\ShaderProgram.h"
#include "Graphics\Pipeline.h"
#include "Graphics\Vertex.h"
#include "Graphics\Material.h"
#include "Graphics\RenderPass.h"
#include "Graphics\Command.h"

struct AppWindow;
struct SDL_Window;

enum class EGraphicsAPI
{
    OPENGL, VULKAN, /*DX12,*/
    MAX
};

struct IGraphicsDevice
    : public Object
{
    BIND_OBJECT(IGraphicsDevice);

    static Ref Create(EGraphicsAPI api);

    void Initialize(std::shared_ptr<AppWindow> window);

    // shader operations
    virtual IShaderProgram::Ref CreateShaderProgram(
          std::string_view path
        , ShaderProgramConfig const& config) = 0;

    virtual IShaderProgram::Ref CreateShaderProgram(
        std::vector<Shader> const& shaders) = 0;

    virtual IShaderTransferBlock::Ref CreateShaderTransferBlock(
          u64 blobSize
        , void const* blob
        , u64 binding) = 0;

    IShaderTransferBlock::Ref GetPerFrameShaderTransferBlock() { return perFrameSTB; }
    IShaderTransferBlock::Ref GetPerObjectShaderTransferBlock() { return perObjectSTB; }

    // pipeline creation
    virtual IPipeline::Ref CreatePipeline(
        PipelineSpecification const& spec) = 0;

    // render pass
    virtual IRenderPass::Ref CreateRenderPass(
        RenderPassSpecification const& spec) = 0;

    virtual IRenderPass::Ref GetDefaultRenderPass() = 0;

    // texture creation
    virtual ITexture::Ref CreateTexture(
          int width
        , int height
        , int components
        , PrimitiveType type
        , void* pixels
        , TextureSpecification const& spec) = 0;

    virtual ITexture::Ref CreateTexture(
          std::string_view path
        , TextureSpecification const& spec) = 0;

    virtual ICommand::Ref BeginFrame() = 0;
    virtual void FinishFrame() = 0;
protected:
    virtual void InitialiseInternal(std::shared_ptr<AppWindow> window) = 0;
private:
    IShaderTransferBlock::Ref perFrameSTB = nullptr;
    IShaderTransferBlock::Ref perObjectSTB = nullptr;
};