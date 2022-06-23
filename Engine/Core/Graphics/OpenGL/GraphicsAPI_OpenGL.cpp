#include "Graphics\OpenGL\GraphicsAPI_OpenGL.h"

#include "Graphics\OpenGL\CoreGL.h"
#include "Graphics\OpenGL\Texture_OpenGL.h"
#include "Graphics\OpenGL\ShaderProgram_OpenGL.h"
#include "Graphics\OpenGL\Material_OpenGL.h"
#include "Graphics\OpenGL\Framebuffer_OpenGL.h"
#include "Graphics\OpenGL\Vertex_OpenGL.h"
#include "Graphics\OpenGL\Pipeline_OpenGL.h"
#include "Graphics\OpenGL\RenderPass_OpenGL.h"
#include "Graphics\OpenGL\Command_OpenGL.h"

#include <SDL.h>
#include <GL/glew.h>
#include <SDL_opengl.h>

namespace gl
{
    GraphicsAPI::GraphicsAPI()
    {
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
#if 0
        //SDL_window* window = SDL_CreateWindow();

        PIXELFORMATDESCRIPTOR pfd =
        {
            sizeof(PIXELFORMATDESCRIPTOR),
            1,
            PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,    // Flags
            PFD_TYPE_RGBA,        // The kind of Framebuffer. RGBA or palette.
            32,                   // Colordepth of the Framebuffer.
            0, 0, 0, 0, 0, 0,
            0,
            0,
            0,
            0, 0, 0, 0,
            32,                   // Number of bits for the depthbuffer
            0,                    // Number of bits for the stencilbuffer
            0,                    // Number of Aux buffers in the Framebuffer.
            PFD_MAIN_PLANE,
            0,
            0, 0, 0
        };

        HDC hdc = GetDC(window->handle);

        // setup the pixel format
        int format = ChoosePixelFormat(hdc, &pfd);
        SetPixelFormat(hdc, format, &pfd);

        // create and setup context
        wglContext = wglCreateContext(hdc);
        wglMakeCurrent(hdc, wglContext);

        glewExperimental = true;
        if (glewInit() != GLEW_OK)
        {
            LOG_ERROR("Failed to initialise GLEW!\n");
        }

        glEnable(GL_TEXTURE_2D);
        glEnable(GL_DEPTH);
        glEnable(GL_DEPTH_TEST);

        LOG("OpenGLVersion(%s)", glGetString(GL_VERSION));
#endif
    }
    
    GraphicsAPI::~GraphicsAPI()
    {
        // TODO(jack): terminate opengl
    }
    
    void GraphicsAPI::InitialiseInternal(std::shared_ptr<AppWindow> window)
    {
        glewExperimental = true;
        if (glewInit() != GLEW_OK)
        {
            LOG_ERROR("Failed to initialise GLEW!");
        }

        glEnable(GL_TEXTURE_2D);
        glEnable(GL_DEPTH);
        glEnable(GL_DEPTH_TEST);

        LOG("OpenGLVersion(%s)", glGetString(GL_VERSION));
    }

    IShaderProgram::Ref GraphicsAPI::CreateShaderProgram(
          std::string_view path
        , ShaderProgramConfig const& config)
    {
        return ShaderProgram::Create(GetGraphicsAPIRef(), path, config);
    }

    IShaderProgram::Ref GraphicsAPI::CreateShaderProgram(
        std::vector<Shader> const& shaders)
    {
        return ShaderProgram::Create(GetGraphicsAPIRef(), shaders);
    }

    IShaderTransferBlock::Ref GraphicsAPI::CreateShaderTransferBlock(
          u64 blobSize
        , void const* blob
        , u64 binding)
    {
        return ShaderTransferBlock::Create(binding, blobSize, blob);
    }

    IPipeline::Ref GraphicsAPI::CreatePipeline(
        PipelineSpecification const& spec)
    {
        return Pipeline::Create(GetGraphicsAPIRef(), spec);
    }

    IRenderPass::Ref GraphicsAPI::CreateRenderPass(
        RenderPassSpecification const& spec)
    {
        return RenderPass::Create(GetGraphicsAPIRef(), spec);
    }
    
    IRenderPass::Ref GraphicsAPI::GetDefaultRenderPass()
    {
        return nullptr;
    }

    ITexture::Ref GraphicsAPI::CreateTexture(
          int width
        , int height
        , int components
        , PrimitiveType type
        , void* pixels
        , TextureSpecification const& spec)
    {
        return Texture::Create(width, height, components, type, pixels, spec);
    }

    ITexture::Ref GraphicsAPI::CreateTexture(
          std::string_view path
        , TextureSpecification const& spec)
    {
        return CreateTextureFromPath(GetGraphicsAPIRef(), path, spec);
    }
    
    ICommand::Ref GraphicsAPI::BeginFrame()
    {
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        return Command::Create();
    }

    void GraphicsAPI::FinishFrame()
    {

    }
}