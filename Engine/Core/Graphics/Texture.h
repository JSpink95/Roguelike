#pragma once
#include "Core.h"
#include "Core\object.h"

ASSET_FWD_DECL(ITexture);
struct IGraphicsDevice;

enum class TextureFilter
{
    POINT, LINEAR
};

enum class TextureWrap
{
    REPEAT, CLAMP
};

struct TextureSpecification
{
    TextureFilter filterMode = TextureFilter::LINEAR;
    TextureWrap wrapMode = TextureWrap::REPEAT;
};

struct ITexture: public Object
{
    BIND_OBJECT(ITexture);
public:
    static std::shared_ptr<ITexture> checkerboard;
    static std::shared_ptr<ITexture> white;

    static void InitialiseDefaultTextures(std::shared_ptr<IGraphicsDevice> api);

    virtual ~ITexture() = default;
};

struct TextureObject
{
    u32 binding = 0u;
    ITexture::Ref texture = nullptr;
};