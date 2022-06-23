#include "Graphics\Vulkan\Command_Vulkan.h"

#ifdef _VULKAN
#include "Graphics\Vulkan\Context_Vulkan.h"

namespace vk
{
    Command::Ref Command::Create(VkCommandBuffer buffer, u32 activeImageIndex)
    {
        return std::make_shared<Command>(buffer, activeImageIndex);
    }

    Command::Command(VkCommandBuffer inCommandBuffer, u32 activeImageIndex)
        : commandBuffer(inCommandBuffer)
        , activeImageIndex(activeImageIndex)
    {

    }
}
#endif