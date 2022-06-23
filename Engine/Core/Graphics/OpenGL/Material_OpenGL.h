#pragma once
#include "Core.h"
#include "Graphics\Material.h"

#include <unordered_map>

namespace gl
{
    struct Material : public IMaterial
    {
        Material(std::shared_ptr<IShaderProgram> shader);
        
        void Bind() final;
        void SetTexture(std::string_view id, std::shared_ptr<ITexture> tex) final;

        std::unordered_map<std::string, std::shared_ptr<IMaterialParameter>> parameters;
    };
}