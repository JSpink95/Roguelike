#pragma once
#include "Core.h"

#ifdef _VULKAN
#include "Core\Object.h"
#include "Graphics\Vulkan\CoreVK.h"

#include <vector>

struct AppWindow;
struct IFramebuffer;

namespace vk
{
    struct Context;
    struct RenderPass_Vulkan;

    struct SwapChainSupportDetails
    {
        VkSurfaceCapabilitiesKHR capabilities;
        std::vector<VkSurfaceFormatKHR> formats;
        std::vector<VkPresentModeKHR> presentModes;
    };

    SwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice device, VkSurfaceKHR surface);

    struct SwapChain : public Object
    {
        BIND_OBJECT(SwapChain);

        static Ref Create(
              std::shared_ptr<Context> context
            , std::shared_ptr<AppWindow> window);

        SwapChain(std::shared_ptr<Context> context);
        virtual ~SwapChain();

        bool InitialiseFramebuffers(std::shared_ptr<RenderPass_Vulkan> renderPass);

        std::shared_ptr<IFramebuffer> GetActiveFramebuffer() { return framebuffers.at(activeFramebufferIndex); }

        std::shared_ptr<Context> context = nullptr;
        VkSwapchainKHR swapChain = VK_NULL_HANDLE;
        std::vector<VkImage> images = {};
        std::vector<VkImageView> imageViews = {};
        std::vector<std::shared_ptr<IFramebuffer>> framebuffers = {};
        VkFormat imageFormat;
        VkExtent2D extent;
        u32 activeFramebufferIndex = 0u;

    private:
        bool InitialiseSwapChain(std::shared_ptr<AppWindow> window);
        bool InitialiseImageViews();
        //bool InitialiseDescriptorPool();
    };
}
#endif