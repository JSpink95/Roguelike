#include "Core\KeyboardInputManager.h"

KeyboardInputManager::Ref KeyboardInputManager::Create()
{
    return std::make_shared<KeyboardInputManager>();
}

void KeyboardInputManager::OnKeyEvent(SDL_KeyboardEvent& keyEvent)
{
    SDL_Keycode keyCode = keyEvent.keysym.sym;
    switch (keyEvent.type)
    {
        case SDL_KEYDOWN:
        {
            auto it = keyListeners.find(keyCode);
            if (it != keyListeners.end())
            {
                for (auto listener : it->second)
                {
                    if (listener.onKeyDown != nullptr)
                        listener.onKeyDown(listener.context, keyCode, keyEvent.repeat);
                }
            }
        } break;

        case SDL_KEYUP:
        {
            auto it = keyListeners.find(keyCode);
            if (it != keyListeners.end())
            {
                for (auto listener : it->second)
                {
                    if (listener.onKeyUp != nullptr)
                        listener.onKeyUp(listener.context, keyCode);
                }
            }
        } break;

        default:
        {
            // :O
        } break;
    }
}

void KeyboardInputManager::AddKeyEventHandler(SDL_Keycode key, void* context, KeyDownFunction onKeyDown, KeyUpFunction onKeyUp)
{
    if (keyListeners.find(key) == keyListeners.end())
    {
        keyListeners.emplace(key, std::vector<KeyEventHandler>{});
    }

    auto& listeners = keyListeners.at(key);

    KeyEventHandler keyEventHandler = { context, onKeyDown, onKeyUp };
    listeners.push_back(keyEventHandler);
}

void KeyboardInputManager::RemoveKeyEventHandler(SDL_Keycode key, void* context)
{
    if (keyListeners.find(key) == keyListeners.end())
    {
        return;
    }

    auto& listeners = keyListeners.at(key);
    std::remove_if(listeners.begin(), listeners.end(), [&context](KeyEventHandler const& handler) -> bool
    {
        return handler.context == context;
    });
}
