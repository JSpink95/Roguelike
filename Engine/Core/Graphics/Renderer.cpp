#include "Graphics\Renderer.h"
#include "Graphics\GraphicsAPI.h"

RendererData::RendererData(std::shared_ptr<IGraphicsDevice> api)
{
    auto per_frame = api->CreateShaderTransferBlock(
          sizeof(PerFrameMatrixShaderBlock)
        , nullptr
        , ShaderTransferBlockIndex::PER_FRAME_MATRICES);

    auto per_object = api->CreateShaderTransferBlock(
          sizeof(PerObjectMatrixShaderBlock)
        , nullptr
        , ShaderTransferBlockIndex::PER_OBJECT_MATRICES);

    transferBlocks.at(ShaderTransferBlockIndex::PER_FRAME_MATRICES) = per_frame;
    transferBlocks.at(ShaderTransferBlockIndex::PER_OBJECT_MATRICES) = per_object;
}