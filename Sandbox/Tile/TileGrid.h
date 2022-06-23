#pragma once
#include <Core.h>
#include <Core\Object.h>

#include <Game\Sprite.h>

#include <box2d\box2d.h>

#include <array>
#include <vector>
#include <functional>

enum class TileGridLayerIndex : u8
{
    Default, Interact, Foreground
    , Max
};

struct TileData : public Object
{
    BIND_OBJECT(TileData);

    Sprite::Ref sprite = nullptr;
};

struct Tile : public Object
{
    BIND_OBJECT(Tile);

    static Ref Create(u32 x, u32 y);
    Tile(u32 x, u32 y);

    u32 x, y;
    Sprite::Ref sprite = nullptr;
};

struct TileGridLayer : public Object
{
    BIND_OBJECT(TileGridLayer);

    static Ref Create(TileGridLayerIndex layer, u32 width, u32 height);

    TileGridLayer(TileGridLayerIndex layer, u32 width, u32 height);

    Tile::Ref GetTile(int x, int y);
    void ForEachTile(std::function<void(Tile::Ref)> func);

    TileGridLayerIndex layerIndex;
    u32 width, height;
    std::vector<Tile::Ref> tiles;
};

struct TileGridSpecification
{
    u32 width = 1u;
    u32 height = 1u;
};

struct TileGrid : public Object
{
    BIND_OBJECT(TileGrid);

    static Ref Create(TileGridSpecification const& spec);

    TileGrid(TileGridSpecification const& spec);

    void ForEachTileInLayer(std::function<void(Tile::Ref)> predicate, TileGridLayerIndex layerIndex);

    TileGridSpecification specification;
    std::array<TileGridLayer::Ref, static_cast<size_t>(TileGridLayerIndex::Max)> layers;
};

// using signed ints so we can index with negative bounds
struct TileGridBounds
{
    int x = 0, y = 0;
    int width = 1, height = 1;
};

// use as temporary fast views into a tile grid
struct TileGridSpan : public Object
{
    BIND_OBJECT(TileGridSpan);

    TileGridSpan(TileGrid::Ref source, TileGridBounds const& bounds);

    TileGrid::Ref tileGridSource;
    TileGridBounds viewBounds;

    void ForEachTileInLayer(std::function<void(Tile::Ref)> predicate, TileGridLayerIndex layerIndex);
};