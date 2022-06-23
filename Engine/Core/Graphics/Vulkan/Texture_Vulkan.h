#pragma once
#include "Core.h"

#ifdef _VULKAN
#include "Graphics\Texture.h"
#include "Graphics\Vulkan\CoreVK.h"

namespace vk
{
    struct Texture : public ITexture
    {
        BIND_OBJECT(Texture);

        static Ref Create(
              std::shared_ptr<Context> context
            , TextureSpecification const& spec);
        static Ref Create(
              std::shared_ptr<Context> context
            , VkImageView imageView);

        Texture(std::shared_ptr<Context> context);
        ~Texture();

        std::shared_ptr<Context> context = nullptr;
        VkImage image = VK_NULL_HANDLE;
        VkImageView imageView = VK_NULL_HANDLE;
    private:
        bool InitialiseTexture(TextureSpecification const& spec);
    };
}
#endif