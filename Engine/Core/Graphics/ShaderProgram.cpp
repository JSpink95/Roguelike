#include "Graphics\ShaderProgram.h"

ShaderProgramConfig ShaderProgramConfig::vert_frag_config = {
    ShaderFlags::VERTEX | ShaderFlags::FRAGMENT
};

ShaderProgramConfig ShaderProgramConfig::vert_geom_frag_config = {
    ShaderFlags::VERTEX | ShaderFlags::GEOMETRY | ShaderFlags::FRAGMENT
};
