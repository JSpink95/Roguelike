#include "Core\window.h"

AppWindow::Ref AppWindow::Create(AppWindowSpecification const& specification)
{
    // initialise SDL
    SDL_Init(SDL_INIT_EVERYTHING);
    
    AppWindow::Ref result = nullptr;

    switch (specification.api)
    {
        case EGraphicsAPI::OPENGL: result = std::make_shared<gl::Window>(specification); break;
        case EGraphicsAPI::VULKAN: result = std::make_shared<vk::Window>(specification); break;
        default:
            LOG_ERROR("Unrecognised API(%u)!\n", (u32)specification.api);
            break;
    }

    if (result)
    {
        result->windowRenderer = SDL_CreateRenderer(result->window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_TARGETTEXTURE);

        // initialise the graphics api
        result->graphics->Initialize(result);
        ITexture::InitialiseDefaultTextures(result->graphics);

        // initialise input
        result->keyboard = KeyboardInputManager::Create();
    }

    return result;
}

ivec2 AppWindow::GetDimensions() const
{
    int x, y;
    SDL_GetWindowSize(window, &x, &y);

    return ivec2(x, y);
}

float AppWindow::GetAspectRatio() const
{
    int x, y;
    SDL_GetWindowSize(window, &x, &y);

    return (float)y / (float)x;
}

ICommand::Ref AppWindow::BeginFrame()
{
    SDL_RenderClear(windowRenderer);

    ICommand::Ref command = nullptr;
    if (graphics)
        command = graphics->BeginFrame();

    return command;
}

void AppWindow::EndFrame()
{
    if (graphics)
        graphics->FinishFrame();

    PollEvents();
    Present();
}

void AppWindow::PollEvents()
{
    SDL_Event windowEvent;
    while (SDL_PollEvent(&windowEvent))
        HandleEvent(&windowEvent);
}

void AppWindow::Present()
{
    SDL_RenderPresent(windowRenderer);
}

void AppWindow::HandleEvent(SDL_Event* windowEvent)
{
    switch (windowEvent->type)
    {
        case SDL_QUIT:
            closeRequested = true;
            break;

        case SDL_WINDOWEVENT:
        {
            switch (windowEvent->window.event)
            {
                case SDL_WINDOWEVENT_CLOSE:
                {
                    closeRequested = true;
                } break;
            }
        } break;

        case SDL_KEYDOWN:
        case SDL_KEYUP:
        {
            keyboard->OnKeyEvent(windowEvent->key);
        } break;
    }
}

namespace gl
{
    Window::Window(AppWindowSpecification const& specification)
    {
        graphics = IGraphicsDevice::Create(EGraphicsAPI::OPENGL);
        window = SDL_CreateWindow(
              specification.title.c_str()
            , SDL_WINDOWPOS_CENTERED
            , SDL_WINDOWPOS_CENTERED
            , specification.width
            , specification.height
            , SDL_WINDOW_OPENGL);
        context = SDL_GL_CreateContext(window);
    }

    void Window::Present()
    {
        SDL_GL_SwapWindow(window);
    }
}

namespace vk
{
    Window::Window(AppWindowSpecification const& specification)
    {
        graphics = IGraphicsDevice::Create(EGraphicsAPI::VULKAN);
        window = SDL_CreateWindow(
              specification.title.c_str()
            , SDL_WINDOWPOS_CENTERED
            , SDL_WINDOWPOS_CENTERED
            , specification.width
            , specification.height
            , SDL_WINDOW_VULKAN);
    }
}