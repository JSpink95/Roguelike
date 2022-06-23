#include "Graphics\OpenGL\ShaderProgram_OpenGL.h"
#include "Graphics\OpenGL\Texture_OpenGL.h"
#include "Graphics\OpenGL\OpenGLUtility.h"

#include "Platform\FileIO.h"

#include <vector>
#include <functional>
#include <algorithm>

namespace gl
{
    namespace util
    {
        std::string ShaderTypeToString(GLenum type)
        {
            switch (type)
            {
                case GL_VERTEX_SHADER: return "VERTEX_SHADER";
                case GL_GEOMETRY_SHADER: return "GEOMETRY_SHADER";
                case GL_FRAGMENT_SHADER:return "FRAGMENT_SHADER";
                default: return "NO_SHADER";
            }
        }

        bool CheckShaderError(GLuint shader)
        {
            GLint isCompiled = 0;
            glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled);
            
            if (isCompiled == GL_FALSE)
            {
                GLint maxLength = 0;
                glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);

                std::vector<GLchar> errorLog(maxLength);
                glGetShaderInfoLog(shader, maxLength, &maxLength, &errorLog[0]);

                LOG_ERROR("SHADER ERROR\n***\n%s", errorLog.data());
            }

            return (isCompiled == GL_TRUE);
        }

        bool CheckProgramError(GLuint program)
        {
            GLint isLinked = 0;
            glGetProgramiv(program, GL_LINK_STATUS, &isLinked);

            if (isLinked == GL_FALSE)
            {
                GLint maxLength = 0;
                glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);

                if (maxLength > 0)
                {
                    // The maxLength includes the NULL character
                    std::vector<GLchar> errorLog(maxLength);
                    glGetProgramInfoLog(program, maxLength, &maxLength, &errorLog[0]);

                    LOG_ERROR("PROGRAM ERROR\n***\n%s\n***", errorLog.data());
                }

            }

            return (isLinked == GL_TRUE);
        }

        std::string EmplaceShaderMacro(std::string_view source, GLenum type)
        {
            std::string result = source.data();
            std::string macro = "\n#define " + ShaderTypeToString(type);

            result.insert(result.find_first_of("\n"), macro);

            return result;
        }

        GLuint CreateShader(std::string_view source, GLenum type)
        {
            GLuint shader = glCreateShader(type);
            std::string adjusted_source = EmplaceShaderMacro(source, type);

            char const* asrc = adjusted_source.c_str();
            glShaderSource(shader, 1, &asrc, nullptr);
            glCompileShader(shader);

            CheckShaderError(shader);

            return shader;
        }

        void LinkProgram(GLuint program)
        {
            glLinkProgram(program);
            CheckProgramError(program);
        }

        GLuint CreateUniformBufferObject(u64 blob_size, void const* blob)
        {
            GLuint ubo;
            glGenBuffers(1, &ubo);
            glBindBuffer(GL_UNIFORM_BUFFER, ubo);
            glBufferData(GL_UNIFORM_BUFFER, (GLsizei)blob_size, blob, GL_DYNAMIC_DRAW);
            glBindBuffer(GL_UNIFORM_BUFFER, 0);
            
            return ubo;
        }

        void SetUniformBufferData(GLuint id, u64 blob_size, void const* blob)
        {
            glBindBuffer(GL_UNIFORM_BUFFER, id);
            glBufferData(GL_UNIFORM_BUFFER, (GLsizei)blob_size, blob, GL_DYNAMIC_DRAW);
            glBindBuffer(GL_UNIFORM_BUFFER, 0);
        }
    }
    
    void ShaderTransferBlock::Set(u64 blobSize, void const* blob)
    {
        util::SetUniformBufferData(id, blobSize, blob);
    }

    void ShaderProgram::Begin()
    {
        glUseProgram(id);
    }

    void ShaderProgram::End()
    {
        glUseProgram(0);
    }

    void ShaderProgram::UploadTexture(std::shared_ptr<ITexture> itex, u64 binding)
    {
        auto tex = std::dynamic_pointer_cast<Texture>(itex);
        glBindTextureUnit(binding, tex->id);
    }

    ShaderProgram::Ref ShaderProgram::Create(
          std::shared_ptr<GraphicsAPI> api
        , std::string_view path
        , ShaderProgramConfig const& config)
    {
        auto result = std::make_shared<ShaderProgram>();

        std::string loaded_file = core::fileio::LoadFileToString(path);

        result->id = glCreateProgram();

        std::vector<GLuint> shaders;
        if (BITFIELD_CHECK(config.bitfield, ShaderFlags::VERTEX))
            shaders.push_back(util::CreateShader(loaded_file, GL_VERTEX_SHADER));

        if (BITFIELD_CHECK(config.bitfield, ShaderFlags::GEOMETRY))
            shaders.push_back(util::CreateShader(loaded_file, GL_GEOMETRY_SHADER));

        if (BITFIELD_CHECK(config.bitfield, ShaderFlags::FRAGMENT))
            shaders.push_back(util::CreateShader(loaded_file, GL_FRAGMENT_SHADER));

        // attach each shader
        for (auto shader : shaders)
            glAttachShader(result->id, shader);

        // link the program
        util::LinkProgram(result->id);

        // detach and destroy all
        for (auto shader : shaders)
        {
            glDetachShader(result->id, shader);
            glDeleteShader(shader);
        }

        return result;
    }

    ShaderProgram::Ref ShaderProgram::Create(
          std::shared_ptr<GraphicsAPI> api
        , std::vector<Shader> const& shaders)
    {
        auto result = std::make_shared<ShaderProgram>();

        if (!result->InitialiseProgram())
        {
            return nullptr;
        }

        if (!result->InitialiseShaders(shaders))
        {
            return nullptr;
        }

        if (!result->LinkProgram())
        {
            return nullptr;
        }

        return result;
    }

    bool ShaderProgram::InitialiseProgram()
    {
        id = glCreateProgram();
        return true;
    }

    bool ShaderProgram::InitialiseShaders(std::vector<Shader> const& shaders)
    {
        constexpr char const* ENTRYPOINT = "main";

        for (auto shader : shaders)
        {
            GLuint shaderId =  glCreateShader(FromShaderFlag(shader.type));

            // KEEP FOR TESTING
#if 0
            auto shaderSource = core::fileio::LoadFileToString(shader.path);
            auto shaderSourceRef = shaderSource.data();

            glShaderSource(shaderId, 1, &shaderSourceRef, nullptr);
            glCompileShader(shaderId);

            util::CheckShaderError(shaderId);
#endif

            auto binary = core::fileio::LoadBinaryFile(shader.path);
            auto bytecode = reinterpret_cast<const u32*>(binary.data());
            glShaderBinary(1, &shaderId, GL_SHADER_BINARY_FORMAT_SPIR_V, bytecode, binary.size());
            glSpecializeShader(shaderId, ENTRYPOINT, 0, nullptr, nullptr);

            shaderIds.push_back(shaderId);
        }

        return true;
    }

    bool ShaderProgram::LinkProgram()
    {
        for (auto shaderId : shaderIds)
        {
            glAttachShader(id, shaderId);
        }

        glLinkProgram(id);
        if (!util::CheckProgramError(id))
        {
            printf("Failed to link program!\n");
            return false;
        }

        for (auto shaderId : shaderIds)
        {
            glDetachShader(id, shaderId);
            glDeleteShader(shaderId);
        }

        return true;
    }
    
    ShaderTransferBlock::Ref ShaderTransferBlock::Create(
          u64 binding
        , u64 blob_size
        , void const* blob)
    {
        auto block = std::make_shared<ShaderTransferBlock>(binding);
        block->id = util::CreateUniformBufferObject(blob_size, blob);

        glBindBufferBase(GL_UNIFORM_BUFFER, binding, block->id);

        return block;
    }
}