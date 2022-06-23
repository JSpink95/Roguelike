#include "Graphics\GraphicsAPI.h"
#include "Graphics\OpenGL\GraphicsAPI_OpenGL.h"
#include "Graphics\Vulkan\GraphicsAPI_Vulkan.h"

IGraphicsDevice::Ref IGraphicsDevice::Create(EGraphicsAPI api)
{
    switch (api)
    {
#ifdef _OPENGL
        case EGraphicsAPI::OPENGL:
            return std::make_shared<gl::GraphicsAPI>();
#endif

#ifdef _VULKAN
        case EGraphicsAPI::VULKAN:
            return std::make_shared<vk::GraphicsAPI>();
#endif

        default:
            LOG_ERROR("Unsupported API type! %i.\n", static_cast<int>(api));
            return nullptr;
    }
}

void IGraphicsDevice::Initialize(std::shared_ptr<AppWindow> window)
{
    InitialiseInternal(window);

    perFrameSTB = CreateShaderTransferBlock(sizeof(PerFrameMatrixShaderBlock), nullptr, 0);
    perObjectSTB = CreateShaderTransferBlock(sizeof(PerObjectMatrixShaderBlock), nullptr, 1);
}