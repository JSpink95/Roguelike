#pragma once
#include "Core.h"

#include <string>

namespace core::string
{
    bool StartWith(std::string_view input, std::string_view prefix);
}