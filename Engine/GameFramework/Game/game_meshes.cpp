#include "Game\game_meshes.h"

#include <Graphics\GraphicsAPI.h>
#include <Graphics\vertex.h>

std::shared_ptr<IVertexArray> Mesh::screenQuad = nullptr;
std::shared_ptr<IVertexArray> Mesh::genericSprite = nullptr;

void Mesh::Initialise(std::shared_ptr<IGraphicsDevice> graphics)
{
    // screen quad
    {
        ScreenVertex screen_verts[6] = {
            { vec2(-1.0f, -1.0f) }, { vec2(1.0f, -1.0f) }, { vec2(1.0f, 1.0f) },
            { vec2(-1.0f, -1.0f) }, { vec2(1.0f,  1.0f) }, { vec2(-1.0f, 1.0f) }, };

        //screenQuad = graphics->CreateVertexArray({ VertexAttributeType::FLOAT2 }, 6u, screen_verts);
    }

    // generic sprite
    {
        SpriteVertex sprite_verts[6] = {
            { vec3(-0.5f, -0.5f, 0.0f), vec2(0.0f, 1.0f) },
            { vec3( 0.5f, -0.5f, 0.0f), vec2(1.0f, 1.0f) },
            { vec3( 0.5f,  0.5f, 0.0f), vec2(1.0f, 0.0f) },

            { vec3(-0.5f, -0.5f, 0.0f), vec2(0.0f, 1.0f) },
            { vec3( 0.5f,  0.5f, 0.0f), vec2(1.0f, 0.0f) },
            { vec3(-0.5f,  0.5f, 0.0f), vec2(0.0f, 0.0f) }, };

        //genericSprite = graphics->CreateVertexArray(
        //      { VertexAttributeType::FLOAT3, VertexAttributeType::FLOAT2 }
        //    , 6u
        //    , sprite_verts);
    }
}

void Mesh::Deinitialise()
{
    screenQuad = nullptr;
    genericSprite = nullptr;
}
