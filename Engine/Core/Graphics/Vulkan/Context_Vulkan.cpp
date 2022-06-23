#include "Graphics\Vulkan\Context_Vulkan.h"

#ifdef _VULKAN
#include "Graphics\Vulkan\SwapChain_Vulkan.h"
#include "Core\Window.h"

#include <set>

namespace vk
{
    QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device, VkSurfaceKHR surface)
    {
        QueueFamilyIndices indices;

        uint32_t queueFamilyCount = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

        std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

        int i = 0;
        for (const auto& queueFamily : queueFamilies)
        {
            if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
                indices.graphicsFamily = i;

            VkBool32 presentSupport = false;
            vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport);

            if (presentSupport)
                indices.presentFamily = i;

            if (indices.IsComplete())
                break;

            i++;
        }

        return indices;
    }

    bool CheckDeviceExtensionSupport(VkPhysicalDevice device)
    {
        return true;
    }

    bool EvaluateDevice(VkPhysicalDevice device, VkSurfaceKHR surface)
    {
        QueueFamilyIndices indices = FindQueueFamilies(device, surface);

        bool extensionsSupported = CheckDeviceExtensionSupport(device);

        bool swapChainAdequate = false;
        if (extensionsSupported)
        {
            SwapChainSupportDetails swapChainSupport = QuerySwapChainSupport(device, surface);
            swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
        }

        return indices.IsComplete() && extensionsSupported && swapChainAdequate;
    }

    static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(
          VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity
        , VkDebugUtilsMessageTypeFlagsEXT messageType
        , const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData
        , void* pUserData)
    {
        LOG_WARNING("Validation Layer: %s", pCallbackData->pMessage);
        return VK_FALSE;
    }

    Context::Ref Context::Create(std::shared_ptr<AppWindow> window)
    {
        auto context = std::make_shared<Context>();
        if (!context->InitialiseInstance())
        {
            return nullptr;
        }

        if (!context->SetupDebugMessenger())
        {
            return nullptr;
        }

        if (!context->InitialiseSurface(window))
        {
            return nullptr;
        }

        if (!context->PickPhysicalDevice())
        {
            return nullptr;
        }

        if (!context->CreateLogicalDevice())
        {
            return nullptr;
        }

        if (!context->CreateCommandPool())
        {
            return nullptr;
        }

        return context;
    }
    
    Context::~Context()
    {

    }

    bool Context::InitialiseInstance()
    {
        VkApplicationInfo appInfo = {};
        appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.pApplicationName = "VulkanApp";
        appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.pEngineName = "No Engine";
        appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.apiVersion = VK_API_VERSION_1_1;

        std::vector<char const*> extensions = { };

        uint32_t extensionCount = 0;
        vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);

        std::vector<VkExtensionProperties> extensionProperties(extensionCount);
        vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensionProperties.data());

        for (auto& extensionProperty : extensionProperties)
        {
            extensions.push_back(extensionProperty.extensionName);
        }

        //extensions.push_back(VK_KHR_MAINTENANCE1_EXTENSION_NAME);

        VkInstanceCreateInfo instanceCreateInfo = {};
        instanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        instanceCreateInfo.pApplicationInfo = &appInfo;
        instanceCreateInfo.enabledExtensionCount = extensions.size();
        instanceCreateInfo.ppEnabledExtensionNames = extensions.data();
        instanceCreateInfo.enabledLayerCount = 0;

        if (vkCreateInstance(&instanceCreateInfo, nullptr, &instance) != VK_SUCCESS)
        {
            LOG_ERROR("Failed to create VulkanInstance!");
            return false;
        }

        return true;
    }

    VkResult CreateDebugUtilsMessengerEXT(
          VkInstance instance
        , const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo
        , const VkAllocationCallbacks* pAllocator
        , VkDebugUtilsMessengerEXT* pDebugMessenger)
    {
        auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
        if (func != nullptr)
        {
            return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
        }
        else
        {
            return VK_ERROR_EXTENSION_NOT_PRESENT;
        }
    }

    bool Context::SetupDebugMessenger()
    {
        VkDebugUtilsMessengerCreateInfoEXT createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
        createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
        createInfo.pfnUserCallback = DebugCallback;
        createInfo.pUserData = nullptr; // Optional

        if (CreateDebugUtilsMessengerEXT(instance, &createInfo, nullptr, &debugMessenger) != VK_SUCCESS)
        {
            LOG_ERROR("Failed to set up debug messenger!");
            return false;
        }

        return true;
    }

    bool Context::InitialiseSurface(std::shared_ptr<AppWindow> window)
    {
        if (SDL_Vulkan_CreateSurface(window->window, instance, &surface) != SDL_TRUE)
        {
            LOG_ERROR("Failed to create a surface!");
            return false;
        }
        
        return true;
    }

    bool Context::PickPhysicalDevice()
    {
        u32 deviceCount = 0;
        vkEnumeratePhysicalDevices(instance, &deviceCount, NULL);

        if (deviceCount == 0)
        {
            LOG_ERROR("Failed to find any physical devices!");
            return false;
        }

        std::vector<VkPhysicalDevice> devices(deviceCount);
        vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

        // get the first device that meets the requirements
        physicalDevice = *std::find_if(devices.begin(), devices.end(), std::bind(EvaluateDevice, std::placeholders::_1, surface));

        return true;
    }

    bool Context::CreateLogicalDevice()
    {
        QueueFamilyIndices indices = FindQueueFamilies(physicalDevice, surface);

        std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
        std::set<u32> uniqueQueueFamilies = { indices.graphicsFamily.value(), indices.presentFamily.value() };

        float queuePriority = 1.0f;
        for (u32 queueFamily : uniqueQueueFamilies)
        {
            VkDeviceQueueCreateInfo queueCreateInfo = {};
            queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            queueCreateInfo.queueFamilyIndex = queueFamily;
            queueCreateInfo.queueCount = 1;
            queueCreateInfo.pQueuePriorities = &queuePriority;
            queueCreateInfos.push_back(queueCreateInfo);
        }

        VkPhysicalDeviceFeatures device_features = {};

        VkDeviceCreateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        createInfo.pQueueCreateInfos = queueCreateInfos.data();
        createInfo.queueCreateInfoCount = (u32)queueCreateInfos.size();

        createInfo.pEnabledFeatures = &device_features;

        std::vector<char const*> device_extensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };
        createInfo.enabledExtensionCount = (u32)device_extensions.size();
        createInfo.ppEnabledExtensionNames = device_extensions.data();

        createInfo.enabledLayerCount = 0;

        if (vkCreateDevice(physicalDevice, &createInfo, nullptr, &device) != VK_SUCCESS)
        {
            LOG_ERROR("Failed to create LogicalDevice!");
            return false;
        }

        vkGetDeviceQueue(device, indices.graphicsFamily.value(), 0, &graphicsQueue);
        vkGetDeviceQueue(device, indices.presentFamily.value(), 0, &presentQueue);
        return true;
    }

    bool Context::CreateCommandPool()
    {
        QueueFamilyIndices queueFamilyIndices = FindQueueFamilies(physicalDevice, surface);

        VkCommandPoolCreateInfo poolInfo = {};
        poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();
        poolInfo.flags = 0; // Optional

        if (vkCreateCommandPool(device, &poolInfo, nullptr, &commandPool) != VK_SUCCESS)
        {
            LOG_ERROR("Failed to create command pool!");
            return false;
        }

        return true;
    }
}
#endif