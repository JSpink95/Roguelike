#include "Graphics\OpenGL\OpenGLUtility.h"

#include "Graphics\ShaderProgram.h"

namespace gl
{
    GLenum FromFilter(TextureFilter filter_mode)
    {
        switch (filter_mode)
        {
            case TextureFilter::LINEAR:
                return GL_LINEAR;
            case TextureFilter::POINT:
                return GL_NEAREST;
            default:
                return GL_NONE;
        }
    }

    GLenum FromWrap(TextureWrap wrap_mode)
    {
        switch (wrap_mode)
        {
            case TextureWrap::REPEAT:
                return GL_REPEAT;
            case TextureWrap::CLAMP:
                return GL_CLAMP_TO_EDGE;
            default:
                return GL_NONE;
        }
    }

    GLenum FromShaderFlag(u64 flag)
    {
        switch (flag)
        {
            case ShaderFlags::VERTEX:
                return GL_VERTEX_SHADER;
            case ShaderFlags::GEOMETRY:
                return GL_GEOMETRY_SHADER;
            case ShaderFlags::FRAGMENT:
                return GL_FRAGMENT_SHADER;
            default:
                return GL_NONE;
        }
    }
}