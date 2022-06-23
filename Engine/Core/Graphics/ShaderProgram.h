#pragma once
#include "Core.h"
#include "Core\Object.h"
#include "Graphics\Texture.h"

ASSET_FWD_DECL(IShaderProgram);

BITFIELD_BEGIN(ShaderFlags)
    BITFIELD_VALUE(VERTEX, 0);
    BITFIELD_VALUE(GEOMETRY, 1);
    BITFIELD_VALUE(FRAGMENT, 2);
BITFIELD_END()

struct ShaderProgramConfig
{
    static ShaderProgramConfig vert_frag_config;
    static ShaderProgramConfig vert_geom_frag_config;

    u64 bitfield = 0u;
};

struct IShaderTransferBlock: public Object
{
    BIND_OBJECT(IShaderTransferBlock);
    virtual void Set(u64 blobSize, void const* blob) = 0;
    virtual ~IShaderTransferBlock() = default;
};

struct Shader
{
    std::string path = "";
    u64 type = 0u;
};

struct IShaderProgram: public Object
{
    BIND_OBJECT(IShaderProgram);
    BIND_ASSET(IShaderProgram);

    virtual ~IShaderProgram() = default;

    virtual void Begin() = 0;
    virtual void End() = 0;

    virtual void UploadTexture(std::shared_ptr<ITexture> tex, u64 binding) = 0;
};