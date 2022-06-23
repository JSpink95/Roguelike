#pragma once
#include "Core.h"
#include "Graphics\ShaderProgram.h"
#include "Graphics\OpenGL\CoreGL.h"

#include <vector>

namespace gl
{
    struct GraphicsAPI;
    
    struct ShaderTransferBlock : public IShaderTransferBlock
    {
        BIND_OBJECT(ShaderTransferBlock);

        static Ref Create(
              u64 binding
            , u64 blobSize
            , void const* blob);

        ShaderTransferBlock(GLuint bind)
            : binding(bind) {}
        ShaderTransferBlock(ShaderTransferBlock const& stb)
            : binding(stb.binding)
            , id(stb.id) {}

        void Set(u64 blobSize, void const* blob) final;

        GLuint const binding;
        GLuint id = 0;
    };

    struct ShaderProgram : public IShaderProgram
    {
        BIND_OBJECT(ShaderProgram);

        static Ref Create(
              std::shared_ptr<GraphicsAPI> api
            , std::string_view path
            , ShaderProgramConfig const& config);

        static Ref Create(
              std::shared_ptr<GraphicsAPI> api
            , std::vector<Shader> const& shaders);

        GLuint id;
        std::vector<GLuint> shaderIds;

        void Begin() final;
        void End() final;

        void UploadTexture(std::shared_ptr<ITexture> tex, u64 binding) final;

    private:
        bool InitialiseProgram();
        bool InitialiseShaders(std::vector<Shader> const& shaders);
        bool LinkProgram();
    };
}