#include "Tile\TileGrid.h"

Tile::Ref Tile::Create(u32 x, u32 y)
{
    return std::make_shared<Tile>(x, y);
}

Tile::Tile(u32 x, u32 y)
    : x(x), y(y)
{
}

TileGridLayer::Ref TileGridLayer::Create(TileGridLayerIndex layer, u32 width, u32 height)
{
    return std::make_shared<TileGridLayer>(layer, width, height);
}

TileGridLayer::TileGridLayer(TileGridLayerIndex layer, u32 width, u32 height)
    : layerIndex(layer)
    , width(width)
    , height(height)
{
    u64 size = (u64)width * (u64)height;
    tiles.reserve(size);

    for (u64 index = 0u; index < size; ++index)
    {
        u32 x = (u32)(index % width);
        u32 y = (u32)(index / width);
        tiles.push_back(Tile::Create(x, y));
    }
}

Tile::Ref TileGridLayer::GetTile(int x, int y)
{
    if (x < 0 || x >= (int)width || y < 0 || y >= (int)height) return nullptr;

    u64 index = (u64)x + (u64)y * (u64)width;
    return tiles.at(index);
}

void TileGridLayer::ForEachTile(std::function<void(Tile::Ref)> func)
{
    for (auto tile : tiles)
        func(tile);
}

TileGrid::Ref TileGrid::Create(TileGridSpecification const& spec)
{
    return std::make_shared<TileGrid>(spec);
}

TileGrid::TileGrid(TileGridSpecification const& spec)
    : specification(spec)
{
    for (u32 layer = 0u; layer < static_cast<u32>(TileGridLayerIndex::Max); ++layer)
    {
        layers.at(layer) = TileGridLayer::Create(static_cast<TileGridLayerIndex>(layer), spec.width, spec.height);
    }
}

void TileGrid::ForEachTileInLayer(std::function<void(Tile::Ref)> predicate, TileGridLayerIndex layerIndex)
{
    layers.at(static_cast<u32>(layerIndex))->ForEachTile(predicate);
}

TileGridSpan::TileGridSpan(TileGrid::Ref source, TileGridBounds const& bounds)
    : tileGridSource(source)
    , viewBounds(bounds)
{
}

void TileGridSpan::ForEachTileInLayer(std::function<void(Tile::Ref)> predicate, TileGridLayerIndex layerIndex)
{
    auto layer = tileGridSource->layers.at(static_cast<u64>(layerIndex));
    if (layer == nullptr)
        return;

    for (int x = viewBounds.x; x < (viewBounds.x + viewBounds.width); ++x)
    {
        for (int y = viewBounds.y; y < (viewBounds.y + viewBounds.height); ++y)
        {
            auto tile = layer->GetTile(x, y);
            if (tile != nullptr)
            {
                predicate(tile);
            }
        }
    }
}