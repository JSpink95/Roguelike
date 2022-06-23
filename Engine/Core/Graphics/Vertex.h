#pragma once
#include "Core.h"
#include "Core\object.h"
#include "Graphics\types.h"

#include <vector>

struct IMaterial;

struct IBuffer : public Object
{
    BIND_OBJECT(IBuffer);
    virtual ~IBuffer() = default;
};

struct IVertexArray: public Object
{
    BIND_OBJECT(IVertexArray);
    virtual ~IVertexArray() = default;

    virtual void UpdateVertexBuffer(u64 vertexCount, void const* vertexData) = 0;
};
