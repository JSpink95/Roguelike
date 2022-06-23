#pragma once
#include "GameCore.h"
#include <Core\Object.h>

struct ISystem: public Object
{
    BIND_OBJECT(ISystem);

    virtual void Update(float delta) = 0;
};
