#pragma once
#include "GameCore.h"
#include <Core\Object.h>

#include <Graphics\GraphicsAPI.h>

struct DebugRenderer : public Object
{
    BIND_OBJECT(DebugRenderer);

    struct InternalData;
    struct Vertex
    {
        static inline VertexAttributeTypeList Attributes =
        {
            VertexAttributeType::FLOAT3, // Position(Vec3)
            VertexAttributeType::FLOAT4, // Color(Vec4)
        };
        vec3 a_Position;
        vec4 a_Color;
    };

    static Ref Create(IGraphicsDevice::Ref graphics);

    DebugRenderer(IGraphicsDevice::Ref graphics);

    void OpenVertexStream();
    void CloseVertexStream();

    void DrawBox2D(vec2 const& min, vec2 const& max, vec4 const& color);
    void DrawCircle2D(vec2 const& centre, float radius, vec4 const& color);
    void DrawLine2D(vec2 const& start, vec2 const& end, vec4 const& color);
    void SubmitVertices(Vertex const* vertices, u32 count);

    IGraphicsDevice::Ref graphics;
    std::unique_ptr<InternalData> internalData;
};
