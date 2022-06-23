#include "Graphics\texture.h"
#include "Graphics\GraphicsAPI.h"

#include <array>

std::shared_ptr<ITexture> ITexture::checkerboard = nullptr;
std::shared_ptr<ITexture> ITexture::white = nullptr;

void ITexture::InitialiseDefaultTextures(std::shared_ptr<IGraphicsDevice> api)
{
    constexpr u8 checkerboardColors[2] = { 255, 155 };

    std::array<u8, 9 * 4> checkerboardPixels;
    for (int index = 0, color = 0; index < 9 * 4; index += 4, color = (color + 1) % 2)
    {
        checkerboardPixels.at((u64)index + 0) = checkerboardColors[(u64)color];
        checkerboardPixels.at((u64)index + 1) = checkerboardColors[(u64)color];
        checkerboardPixels.at((u64)index + 2) = checkerboardColors[(u64)color];
        checkerboardPixels.at((u64)index + 3) = 255;
    }

    std::array<u8, 16 * 4> whitePixels;
    std::fill(whitePixels.begin(), whitePixels.end(), 255);

    TextureSpecification devTextureSpec;
    devTextureSpec.filterMode = TextureFilter::POINT;

    checkerboard = api->CreateTexture(3, 3, 4, PrimitiveType::UINT8, checkerboardPixels.data(), devTextureSpec);
    white = api->CreateTexture(4, 4, 4, PrimitiveType::UINT8, whitePixels.data(), devTextureSpec);
}