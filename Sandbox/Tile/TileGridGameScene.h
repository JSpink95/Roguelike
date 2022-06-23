#pragma once
#include <Core.h>
#include <Core\Object.h>

#include <Game\Scene.h>

#include "Tile\TileGrid.h"

#if 0
struct TileGridGameScene : public SceneWithPhysics2D
{
    BIND_OBJECT(TileGridGameScene);

    static Ref Create();

    TileGridGameScene();
    void Begin() override;
    void InitialiseTile(Tile::Ref tile);

    TileGrid::Ref tileGrid = nullptr;
};
#endif
