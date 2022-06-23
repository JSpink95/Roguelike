#include "Core\StringUtils.h"

namespace core::string
{
    bool StartWith(std::string_view input, std::string_view prefix)
    {
        if (prefix.length() > input.length())
            return false;

        return input.rfind(prefix, 0u) == 0;
    }
}