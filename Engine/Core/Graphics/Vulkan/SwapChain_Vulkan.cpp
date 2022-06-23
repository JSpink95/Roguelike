#include "Graphics\Vulkan\SwapChain_Vulkan.h"

#ifdef _VULKAN
#include "Graphics\Vulkan\Context_Vulkan.h"
#include "Graphics\Vulkan\RenderPass_Vulkan.h"
#include "Graphics\Vulkan\Texture_Vulkan.h"

#include "Core\Window.h"

namespace vk
{
    SwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice device, VkSurfaceKHR surface)
    {
        SwapChainSupportDetails details;
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.capabilities);

        u32 formatCount;
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr);

        if (formatCount != 0)
        {
            details.formats.resize(formatCount);
            vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, details.formats.data());
        }

        u32 presentModeCount;
        vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, nullptr);

        if (presentModeCount != 0)
        {
            details.presentModes.resize(presentModeCount);
            vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, details.presentModes.data());
        }

        return details;
    }

    VkSurfaceFormatKHR ChooseSwapSurfaceFormat(std::vector<VkSurfaceFormatKHR> const& availableFormats)
    {
        for (const auto& availableFormat : availableFormats)
        {
            if (availableFormat.format == VK_FORMAT_R8G8B8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
            //if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
            {
                return availableFormat;
            }
        }

        return availableFormats.at(0);
    }

    VkPresentModeKHR ChooseSwapPresentMode(std::vector<VkPresentModeKHR> const& availablePresentModes)
    {
        for (const auto& availablePresentMode : availablePresentModes)
        {
            if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR)
                return availablePresentMode;
        }

        return VK_PRESENT_MODE_FIFO_KHR;
    }

    VkExtent2D ChooseSwapExtent(VkSurfaceCapabilitiesKHR const& capabilities, std::shared_ptr<AppWindow> window)
    {
        if (capabilities.currentExtent.width != UINT32_MAX)
        {
            return capabilities.currentExtent;
        }
        else
        {
            auto size = window->GetDimensions();

            VkExtent2D actualExtent =
            {
                static_cast<u32>(size.x),
                static_cast<u32>(size.y)
            };

            actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
            actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

            return actualExtent;
        }
    }

    SwapChain::Ref SwapChain::Create(
          std::shared_ptr<Context> context
        , std::shared_ptr<AppWindow> window)
    {
        auto result = std::make_shared<SwapChain>(context);

        if (!result->InitialiseSwapChain(window))
        {
            return nullptr;
        }

        if (!result->InitialiseImageViews())
        {
            return nullptr;
        }

        //if (!result->InitialiseDescriptorPool())
        //{
        //    return nullptr;
        //}

        return result;
    }

    SwapChain::SwapChain(std::shared_ptr<Context> context)
        : context(context)
    {

    }

    SwapChain::~SwapChain()
    {

    }

    bool SwapChain::InitialiseSwapChain(std::shared_ptr<AppWindow> window)
    {
        SwapChainSupportDetails swapchainSupport = QuerySwapChainSupport(context->physicalDevice, context->surface);

        VkSurfaceFormatKHR surfaceFormat = ChooseSwapSurfaceFormat(swapchainSupport.formats);
        VkPresentModeKHR presentMode = ChooseSwapPresentMode(swapchainSupport.presentModes);
        VkExtent2D extent = ChooseSwapExtent(swapchainSupport.capabilities, window);

        u32 imageCount = swapchainSupport.capabilities.minImageCount + 1;
        if (swapchainSupport.capabilities.maxImageCount > 0 && imageCount > swapchainSupport.capabilities.maxImageCount)
        {
            imageCount = swapchainSupport.capabilities.maxImageCount;
        }

        VkSwapchainCreateInfoKHR createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        createInfo.surface = context->surface;
        createInfo.minImageCount = imageCount;
        createInfo.imageFormat = surfaceFormat.format;
        createInfo.imageColorSpace = surfaceFormat.colorSpace;
        createInfo.imageExtent = extent;
        createInfo.imageArrayLayers = 1;
        createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

        QueueFamilyIndices indices = FindQueueFamilies(context->physicalDevice, context->surface);
        uint32_t queueFamilyIndices[] = { indices.graphicsFamily.value(), indices.presentFamily.value() };

        if (indices.graphicsFamily != indices.presentFamily)
        {
            createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
            createInfo.queueFamilyIndexCount = 2;
            createInfo.pQueueFamilyIndices = queueFamilyIndices;
        }
        else
        {
            createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
            createInfo.queueFamilyIndexCount = 0; // Optional
            createInfo.pQueueFamilyIndices = nullptr; // Optional
        }

        createInfo.preTransform = swapchainSupport.capabilities.currentTransform;
        createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        createInfo.presentMode = presentMode;
        createInfo.clipped = VK_TRUE;
        createInfo.oldSwapchain = VK_NULL_HANDLE;

        if (vkCreateSwapchainKHR(context->device, &createInfo, nullptr, &swapChain) != VK_SUCCESS)
        {
            LOG_ERROR("Failed to create a SwapChain!\n");
            return false;
        }

        vkGetSwapchainImagesKHR(context->device, swapChain, &imageCount, nullptr);

        images.resize(imageCount);
        vkGetSwapchainImagesKHR(context->device, swapChain, &imageCount, images.data());

        imageFormat = surfaceFormat.format;
        this->extent = extent;
        return true;
    }

    bool SwapChain::InitialiseImageViews()
    {
        imageViews.resize(images.size());
        for (u64 i = 0; i < images.size(); i++)
        {
            VkImageViewCreateInfo createInfo = {};
            createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            createInfo.image = images[i];
            createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
            createInfo.format = imageFormat;
            createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            createInfo.subresourceRange.baseMipLevel = 0;
            createInfo.subresourceRange.levelCount = 1;
            createInfo.subresourceRange.baseArrayLayer = 0;
            createInfo.subresourceRange.layerCount = 1;

            if (vkCreateImageView(context->device, &createInfo, nullptr, &imageViews[i]) != VK_SUCCESS)
            {
                LOG_ERROR("Failed to create imageview [%i]!\n", i);
                return false;
            }
        }

        return true;
    }

    bool SwapChain::InitialiseFramebuffers(std::shared_ptr<RenderPass_Vulkan> renderPass)
    {
        for (u64 i = 0; i < imageViews.size(); i++)
        {
            FramebufferSpecification::Attachment attachment;
            attachment.texture = Texture::Create(context, imageViews[i]);

            framebuffers.push_back(renderPass->CreateFramebuffer({ attachment }));
        }

        return true;
    }

    //bool SwapChain::InitialiseDescriptorPool()
    //{
    //    VkDescriptorPoolSize poolSize = {};
    //    poolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    //    poolSize.descriptorCount = static_cast<uint32_t>(images.size());
    //
    //    VkDescriptorPoolCreateInfo poolInfo = {};
    //    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    //    poolInfo.poolSizeCount = 1;
    //    poolInfo.pPoolSizes = &poolSize;
    //    poolInfo.maxSets = static_cast<uint32_t>(images.size()) * 2;
    //
    //    if (vkCreateDescriptorPool(context->device, &poolInfo, nullptr, &descriptorPool) != VK_SUCCESS)
    //    {
    //        LOG_ERROR("Failed to create descriptor pool!\n");
    //        return false;
    //    }
    //
    //    return true;
    //}
}
#endif