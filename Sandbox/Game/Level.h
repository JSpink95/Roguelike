#pragma once
#include <Core.h>

#include <vector>

struct LevelHeader
{
    u8 width = 0u;      // max size 256
    u8 height = 0u;     // max size 256
    u8 layers = 0u;     // max size 256
};

// simple collision box descriptor, contains the tile ids for the surrounding box
union LevelCollisionBox
{
    u32 data = 0u;
    struct
    {
        u8 bottomLeft, bottomRight;
        u8 topLeft, topRight;
    };
};

struct LevelLayer
{
    std::vector<u8> tiles = {};
};

struct LevelCollision
{
    std::vector<LevelCollisionBox> colliders = {};
};

struct LevelData
{
    LevelHeader header = {};
    std::vector<LevelLayer> layers = {};
    LevelCollision collision = {};
};

bool LoadLevelFile(std::string_view path, LevelData& out);
bool SaveLevelFile(std::string_view path, LevelData const& level);

// helper function for the above
bool SaveLevelFile(
      std::string_view path
    , u8 width
    , u8 height
    , std::vector<LevelLayer> const& layers
    , std::vector<ivec4> const& collision);
