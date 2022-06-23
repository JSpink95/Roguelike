#include "Tile\TileGridGameScene.h"

#if 0
TileGridGameScene::Ref TileGridGameScene::Create()
{
    return std::make_shared<TileGridGameScene>();
}

TileGridGameScene::TileGridGameScene()
    : Super(nullptr)
{

}

void TileGridGameScene::Begin()
{
    SceneWithPhysics2D::Begin();

    // create all the geometry collision in the physics world.

    for (auto layer : tileGrid->layers)
    {
        layer->ForEachTile(std::bind(&TileGridGameScene::InitialiseTile, this, std::placeholders::_1));
    }
}

void TileGridGameScene::InitialiseTile(Tile::Ref tile)
{
    if (tile->data.collidable)
    {
        tile->physics = std::make_shared<TilePhysics>();

        b2BodyDef bodyDef;
        bodyDef.type = b2BodyType::b2_staticBody;
        bodyDef.position.Set((float)tile->x, (float)tile->y);
        bodyDef.angle = 0.0f;

        auto body = physicsWorld->CreateBody(&bodyDef);
        body->SetFixedRotation(true);
        tile->physics->runtimeBody = body;

        b2PolygonShape shape;
        shape.SetAsBox(1.0f, 1.0f);

        b2FixtureDef fixtureDef;
        fixtureDef.shape = &shape;
        //fixtureDef.density = collider.physMaterial.density;
        //fixtureDef.friction = collider.physMaterial.friction;
        //fixtureDef.restitution = collider.physMaterial.restitution;
        //fixtureDef.restitutionThreshold = collider.physMaterial.restitutionThreshold;

        auto fixture = body->CreateFixture(&fixtureDef);
        tile->physics->runtimeShape = fixture;
    }
}
#endif
