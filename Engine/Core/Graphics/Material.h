#pragma once
#include "Core.h"
#include "Core\object.h"
#include "Graphics\ShaderProgram.h"
#include "Graphics\renderer.h"

#include <json11\json11.hpp>
#include <unordered_map>

ASSET_FWD_DECL(IMaterial);

struct IMaterialParameter: public Object
{
    BIND_OBJECT(IMaterialParameter);

    virtual void upload_to_shader(IShaderProgram::Ref shader) = 0;

    virtual ~IMaterialParameter() = default;
};

struct MaterialParameter_Texture : public IMaterialParameter
{
    BIND_OBJECT(MaterialParameter_Texture);

    void upload_to_shader(IShaderProgram::Ref shader) final;

    std::shared_ptr<ITexture> texture = nullptr;
    u64 binding = 0u;
};

struct IMaterial: public Object
{
    BIND_OBJECT(IMaterial);
    BIND_ASSET(IMaterial);

    IMaterial(IShaderProgram::Ref shader);

    virtual void Bind() = 0;
    virtual void SetTexture(
          std::string_view id
        , ITexture::Ref tex) = 0;

    IShaderProgram::Ref shader;
};
