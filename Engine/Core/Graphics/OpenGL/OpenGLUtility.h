#pragma once
#include "Core.h"
#include "Graphics\OpenGL\CoreGL.h"

#include "Graphics\Texture.h"
#include "Graphics\Framebuffer.h"

namespace gl
{
    GLenum FromFilter(TextureFilter filter_mode);
    GLenum FromWrap(TextureWrap wrap_mode);
    GLenum FromShaderFlag(u64 flag);
}