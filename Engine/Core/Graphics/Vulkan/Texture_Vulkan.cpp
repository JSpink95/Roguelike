#include "Graphics\Vulkan\Texture_Vulkan.h"

#ifdef _VULKAN
#include "Graphics\Vulkan\Context_Vulkan.h"

namespace vk
{
    Texture::Ref Texture::Create(
          std::shared_ptr<Context> context
        , TextureSpecification const& spec)
    {
        auto result = std::make_shared<Texture>(context);

        if (!result->InitialiseTexture(spec))
        {
            return nullptr;
        }

        return result;
    }

    Texture::Ref Texture::Create(
          std::shared_ptr<Context> context
        , VkImageView imageView)
    {
        auto result = std::make_shared<Texture>(context);
        result->imageView = imageView;

        return result;
    }

    Texture::Texture(std::shared_ptr<Context> context)
        : context(context)
    {

    }

    Texture::~Texture()
    {
        if (image != VK_NULL_HANDLE)
            vkDestroyImage(context->device, image, nullptr);
    }

    bool Texture::InitialiseTexture(TextureSpecification const& spec)
    {
        // TODO(jack): implement image loading
        return false;
    }
}
#endif