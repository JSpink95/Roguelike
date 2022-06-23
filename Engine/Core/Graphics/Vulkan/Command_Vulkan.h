#pragma once
#include "Core.h"

#ifdef _VULKAN
#include "Graphics\Command.h"
#include "Graphics\Vulkan\CoreVK.h"

namespace vk
{
    struct Command : public ICommand
    {
        BIND_OBJECT(Command);

        static Ref Create(VkCommandBuffer buffer, u32 activeImageIndex);

        Command(VkCommandBuffer inCommandBuffer, u32 activeImageIndex);

        VkCommandBuffer commandBuffer = VK_NULL_HANDLE;
        u32 activeImageIndex = 0u;

        operator VkCommandBuffer& () { return commandBuffer; }
    };
}
#endif