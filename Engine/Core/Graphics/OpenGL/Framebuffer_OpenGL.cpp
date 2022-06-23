#include "Graphics\OpenGL\Framebuffer_OpenGL.h"
#include "Graphics\OpenGL\Texture_OpenGL.h"
#include "Graphics\OpenGL\GraphicsAPI_OpenGL.h"
#include "Graphics\OpenGL\OpenGLUtility.h"

#include <numeric>

namespace gl
{
    namespace utils
    {
        GLenum GetFormat(GLenum attachment)
        {
            switch (attachment)
            {
                case GL_DEPTH24_STENCIL8:
                    return GL_DEPTH_STENCIL;
                case GL_DEPTH_COMPONENT24:
                case GL_DEPTH_COMPONENT32:
                case GL_DEPTH_COMPONENT32F:
                    return GL_DEPTH_COMPONENT;
                case GL_RGB:
                    return GL_RGB;
                case GL_RGBA:
                    return GL_RGBA;
                case GL_RGB32F:
                    return GL_RGB;
                case GL_RGBA32F:
                    return GL_RGBA;
                default:
                    return GL_NONE;
            }
        }

        GLenum GetTypeFromFormat(GLenum format)
        {
            switch (format)
            {
                case GL_RGB32F:
                case GL_RGBA32F:
                case GL_DEPTH_COMPONENT24:
                case GL_DEPTH_COMPONENT32:
                case GL_DEPTH_COMPONENT32F:
                    return GL_FLOAT;
                default:
                case GL_RGB:
                case GL_RGBA:
                case GL_DEPTH24_STENCIL8:
                    return GL_UNSIGNED_BYTE;
            }
        }

        GLenum GetAttachment(GLenum attachment)
        {
            switch (attachment)
            {
                case GL_DEPTH24_STENCIL8:
                    return GL_DEPTH_STENCIL_ATTACHMENT;
                case GL_DEPTH_COMPONENT:
                case GL_DEPTH_COMPONENT24:
                case GL_DEPTH_COMPONENT32:
                case GL_DEPTH_COMPONENT32F:
                    return GL_DEPTH_ATTACHMENT;
                case GL_RGB:
                case GL_RGBA:
                case GL_RGB32F:
                case GL_RGBA32F:
                    return GL_COLOR_ATTACHMENT0;
                default:
                    return GL_NONE;
            }
        }

        GLuint CreateDepthAttachment(GLuint framebuffer, GLenum format, u32 width, u32 height)
        {
            GLuint id;
            glGenTextures(1, &id);
            glBindTexture(GL_TEXTURE_2D, id);

            glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, GetFormat(format), GetTypeFromFormat(format), nullptr);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_NONE);
            glBindTexture(GL_TEXTURE_2D, 0);

            glFramebufferTexture(GL_FRAMEBUFFER, GetAttachment(format), id, 0);
            return id;
        }
    }

    Framebuffer::Ref Framebuffer::Create(FramebufferSpecification const& specification)
    {
        auto result = std::make_shared<Framebuffer>(specification.width, specification.height);

        if (!result->InitialiseFramebuffer(specification))
        {
            return nullptr;
        }

        return result;
    }

    Framebuffer::Framebuffer(u32 width, u32 height)
        : dimensions(width, height)
    {

    }
    Framebuffer::~Framebuffer()
    {

    }

    bool Framebuffer::InitialiseFramebuffer(FramebufferSpecification const& specification)
    {
        // no attachments is invalid
        if (specification.attachments.size() == 0)
        {
            LOG_ERROR("Failed to initialise framebuffer! No attachments specified.\n");
            return false;
        }

        // no texture or specification means we're creating a "stub" framebuffer for opengl,
        // essentially we will just want to draw directly to the screen.
        if (!specification.attachments.at(0).specification.has_value() && specification.attachments.at(0).texture == nullptr)
            return true;

        glCreateFramebuffers(1, &id);
        glBindFramebuffer(GL_FRAMEBUFFER, id);

        for (auto attachmentSpec : specification.attachments)
        {
            auto attachment = Texture::Create(
                  dimensions.x
                , dimensions.y
                , 4
                , PrimitiveType::UINT8
                , nullptr
                , attachmentSpec.specification.value());

            glFramebufferTexture(
                  GL_FRAMEBUFFER
                , GL_COLOR_ATTACHMENT0 + colorAttachments.size()
                , attachment->id
                , 0);

            colorAttachments.emplace_back(attachment);
        }

        //if (specification.enableDepth)
        //{
        //    GLuint attachment = utils::create_depth_attachment(result->id, GL_DEPTH_COMPONENT32F, specification.dimensions.x, specification.dimensions.y);
        //    result->depthAttachment = attachment;
        //}

        if (colorAttachments.size() > 0)
        {
            std::vector<GLenum> buffers(colorAttachments.size());
            std::iota(buffers.begin(), buffers.end(), GL_COLOR_ATTACHMENT0);

            glDrawBuffers((GLsizei)buffers.size(), buffers.data());
        }
        else
        {
            glDrawBuffer(GL_NONE);
        }

        glBindFramebuffer(GL_FRAMEBUFFER, 0);

    }

    ITexture::Ref Framebuffer::GetColorAttachment(FramebufferSpecification::Attachment::Index index)
    {
        return colorAttachments.at(static_cast<u64>(index));
    }

}