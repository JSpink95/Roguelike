#pragma once
#include "Core.h"
#include "Core\Object.h"
#include "Core\KeyboardInputManager.h"
#include "Graphics\GraphicsAPI.h"

#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_vulkan.h>

struct AppWindowSpecification
{
    std::string title = "";
    u32 width = 0u;
    u32 height = 0u;
    EGraphicsAPI api = EGraphicsAPI::OPENGL;
};

struct AppWindow : public Object
{
    BIND_OBJECT(AppWindow);

    static Ref Create(AppWindowSpecification const& specification);

    ivec2 GetDimensions() const;
    float GetAspectRatio() const;

    ICommand::Ref BeginFrame();
    void EndFrame();

    void PollEvents();
    virtual void Present();
    void HandleEvent(SDL_Event* windowEvent);

    IGraphicsDevice::Ref graphics = nullptr;
    KeyboardInputManager::Ref keyboard = nullptr;
    SDL_Window* window = nullptr;
    SDL_Renderer* windowRenderer = nullptr;
    bool closeRequested = false;
};

namespace gl
{
    struct Window : public AppWindow
    {
        BIND_OBJECT(Window);

        Window(AppWindowSpecification const& specification);
        void Present() override;

        SDL_GLContext context = nullptr;
    };
}

namespace vk
{
    struct Window : public AppWindow
    {
        BIND_OBJECT(Window);

        Window(AppWindowSpecification const& specification);
    };
}