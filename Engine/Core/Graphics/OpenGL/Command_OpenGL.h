#pragma once
#include "Core.h"
#include "Graphics\Command.h"
#include "Graphics\OpenGL\CoreGL.h"

namespace gl
{
    struct Command : public ICommand
    {
        BIND_OBJECT(Command);

        static inline Ref Create() { return std::make_shared<Command>(); }
    };
}