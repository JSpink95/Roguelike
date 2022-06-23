#include "Game\Level.h"

#include <fstream>

bool LoadLevelFile(std::string_view path, LevelData& out)
{
    std::ifstream file(path.data(), std::ios::binary);
    if (!file.is_open())
        return false;

    file.read(ReCast<char>(&out.header), sizeof(LevelHeader));

    u64 const mapSize = (u64)out.header.width * (u64)out.header.height;
    u64 const layerSize = (u64)out.header.layers;

    if (layerSize > 0u)
    {
        out.layers.resize(layerSize);
        for (LevelLayer& layer : out.layers)
        {
            layer.tiles.resize(mapSize);
            file.read(ReCast<char>(layer.tiles.data()), mapSize);
        }
    }

    u32 colliderCount = 0u;
    file.read(ReCast<char>(&colliderCount), sizeof(u32));

    if (colliderCount > 0u)
    {
        out.collision.colliders.resize(colliderCount);
        file.read(ReCast<char>(out.collision.colliders.data()), sizeof(LevelCollisionBox) * colliderCount);
    }

    file.close();
    return true;
}

bool SaveLevelFile(std::string_view path, LevelData const& level)
{
    std::ofstream file(path.data(), std::ios::binary);
    if (!file.is_open())
        return false;

    u64 const mapSize = (u64)level.header.width * (u64)level.header.height;

    file.write(ReCast<char const>(&level.header), sizeof(LevelHeader));

    for (LevelLayer const& layer : level.layers)
    {
        file.write(ReCast<char const>(layer.tiles.data()), mapSize);
    }

    u32 const colliderCount = level.collision.colliders.size();
    file.write(ReCast<char const>(&colliderCount), sizeof(u32));

    if (colliderCount > 0u)
    {
        file.write(ReCast<char const>(level.collision.colliders.data()), sizeof(LevelCollisionBox) * colliderCount);
    }

    file.close();
    return true;
}

bool SaveLevelFile(
      std::string_view path
    , u8 width
    , u8 height
    , std::vector<LevelLayer> const& layers
    , std::vector<ivec4> const& collision)
{
    LevelData level = {};
    
    // setup the header
    level.header.width = width;
    level.header.height = height;
    level.header.layers = (u8)layers.size();

    // setup the layers
    level.layers = layers;

    // setup the collision
    for (auto const& collider : collision)
    {
        LevelCollisionBox box = {};
        box.bottomLeft = (u8)collider.x;
        box.bottomRight = (u8)collider.y;
        box.topLeft = (u8)collider.z;
        box.topRight = (u8)collider.w;

        level.collision.colliders.push_back(box);
    }

    // finally pass this to the save function
    return SaveLevelFile(path, level);
}