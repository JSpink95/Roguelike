#pragma once
#include "Core.h"

#ifdef _VULKAN
#include "Core\Object.h"
#include "Graphics\Vulkan\CoreVK.h"

#include <optional>

struct AppWindow;

namespace vk
{
    struct QueueFamilyIndices
    {
        std::optional<u32> graphicsFamily;
        std::optional<u32> presentFamily;

        inline bool IsComplete() const
        {
            return graphicsFamily.has_value() && presentFamily.has_value();
        }
    };

    QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device, VkSurfaceKHR surface);

    struct Context: public Object
    {
        BIND_OBJECT(Context);

        static Ref Create(std::shared_ptr<AppWindow> window);

        virtual ~Context();

        VkInstance instance = VK_NULL_HANDLE;
        VkDebugUtilsMessengerEXT debugMessenger = VK_NULL_HANDLE;
        VkSurfaceKHR surface = VK_NULL_HANDLE;
        VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
        VkDevice device = VK_NULL_HANDLE;
        VkQueue graphicsQueue = VK_NULL_HANDLE;
        VkQueue presentQueue = VK_NULL_HANDLE;
        VkCommandPool commandPool = VK_NULL_HANDLE;

    private:
        bool InitialiseInstance();
        bool SetupDebugMessenger();
        bool InitialiseSurface(std::shared_ptr<AppWindow> window);
        bool PickPhysicalDevice();
        bool CreateLogicalDevice();
        bool CreateCommandPool();
    };
}
#endif