#pragma once
#include "Core.h"
#include "Core\Object.h"

struct ICommand : public Object
{
    BIND_OBJECT(ICommand);
};