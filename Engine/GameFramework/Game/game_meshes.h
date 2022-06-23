#pragma once
#include "GameCore.h"

struct IGraphicsDevice;
struct IVertexArray;

struct ScreenVertex
{
    vec2 a_Position;
};

struct SpriteVertex
{
    vec3 a_Position;
    vec2 a_Texcoord;
};

struct Mesh
{
    static std::shared_ptr<IVertexArray> screenQuad;
    static std::shared_ptr<IVertexArray> genericSprite;

    static void Initialise(std::shared_ptr<IGraphicsDevice> graphics);
    static void Deinitialise();
};
