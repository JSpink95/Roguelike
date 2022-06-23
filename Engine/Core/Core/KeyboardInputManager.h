#pragma once
#include "Core.h"
#include "Core\Object.h"

#include <functional>
#include <unordered_map>

#include <SDL.h>

using KeyDownFunction = std::function<void(void*, SDL_Keycode, bool)>;
using KeyUpFunction = std::function<void(void*, SDL_Keycode)>;

struct KeyEventHandler
{
    void* context = nullptr;
    KeyDownFunction onKeyDown = nullptr;
    KeyUpFunction onKeyUp = nullptr;
};

struct KeyboardInputManager
    : public Object
{
    BIND_OBJECT(KeyboardInputManager);

    static Ref Create();

    void OnKeyEvent(SDL_KeyboardEvent& keyEvent);

    void AddKeyEventHandler(SDL_Keycode key, void* context, KeyDownFunction onKeyDown, KeyUpFunction onKeyUp);
    void RemoveKeyEventHandler(SDL_Keycode key, void* context);

    std::unordered_map<SDL_Keycode, std::vector<KeyEventHandler>> keyListeners;
};