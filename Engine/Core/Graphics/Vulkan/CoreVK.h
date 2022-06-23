#pragma once
#include <Core.h>

#ifdef _VULKAN
#include <vulkan\vulkan.hpp>

#define VK_USE_PLATFORM_WIN32_KHR
#include <Windows.h>
#include <vulkan\vulkan_win32.h>

#include <SDL.h>
#include <SDL_vulkan.h>

// forward declarations
// TODO(jack): should I forward declare shit here, or just outright incldude the most important stuff? e.g.: COntext_Vulkan?
namespace vk
{
    struct Context;
    struct SwapChain;
}
#endif