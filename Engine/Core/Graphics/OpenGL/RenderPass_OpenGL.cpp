#include "Graphics\OpenGL\RenderPass_OpenGL.h"
#include "Graphics\OpenGL\Framebuffer_OpenGL.h"
#include "Graphics\OpenGL\GraphicsAPI_OpenGL.h"
#include "Graphics\OpenGL\Command_OpenGL.h"

#include <algorithm>
#include <iterator>

namespace gl
{
    namespace utils
    {
        FramebufferSpecification::Attachment ConvertToFramebufferAttachment(RenderPassSpecification::Attachment const& attachment)
        {
            TextureSpecification textureSpec = {};
            textureSpec.filterMode = TextureFilter::POINT;
            textureSpec.wrapMode = TextureWrap::CLAMP;

            FramebufferSpecification::Attachment result = {};
            result.specification = textureSpec;

            return result;
        }
    }
    RenderPass::Ref RenderPass::Create(
          std::shared_ptr<GraphicsAPI> graphics
        , RenderPassSpecification const& spec)
    {
        auto result = std::make_shared<RenderPass>(graphics, spec);

        return result;
    }
    
    RenderPass::RenderPass(std::shared_ptr<GraphicsAPI> graphics, RenderPassSpecification const& spec)
        : graphics(graphics)
        , specification(spec)
    {

    }

    ICommand::Ref RenderPass::Begin(IFramebuffer::Ref framebuffer)
    {
        if (auto fb = Framebuffer::Cast(framebuffer))
        {
            glBindFramebuffer(GL_FRAMEBUFFER, fb->id);
        }

        // TODO(jack): add depth enabled option to spec
        if (true)
        {
            glEnable(GL_DEPTH);
            glEnable(GL_DEPTH_TEST);
        }

        glViewport(0, 0, specification.size.x, specification.size.y);

        glClearColor(specification.clearColor.r, specification.clearColor.g, specification.clearColor.b, specification.clearColor.a);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        return Command::Create();
    }

    void RenderPass::End()
    {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
    
    IFramebuffer::Ref RenderPass::CreateFramebuffer()
    {
        FramebufferSpecification framebufferSpecification = {};
        std::transform(specification.attachments.begin(), specification.attachments.end(), std::back_inserter(framebufferSpecification.attachments), utils::ConvertToFramebufferAttachment);
        framebufferSpecification.width = specification.size.x;
        framebufferSpecification.height = specification.size.y;
        framebufferSpecification.renderPass = GetRenderPassRef();

        return Framebuffer::Create(framebufferSpecification);
    }

    IFramebuffer::Ref RenderPass::CreateFramebuffer(std::vector<FramebufferSpecification::Attachment> const& attachments)
    {
        FramebufferSpecification framebufferSpecification = {};
        framebufferSpecification.attachments = attachments;
        framebufferSpecification.width = specification.size.x;
        framebufferSpecification.height = specification.size.y;
        framebufferSpecification.renderPass = GetRenderPassRef();

        return Framebuffer::Create(framebufferSpecification);
    }
}