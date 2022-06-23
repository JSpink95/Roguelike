#pragma once
#include "Core.h"

namespace core
{
    template<typename T>
    class span
    {
    public:
        span(T* ptr, size_t offset, size_t length)
            : ptr(ptr + offset), length(length) {}

        inline auto begin() { return ptr; }
        inline auto end() { return ptr + length; }
        inline auto size() { return length; }
        inline auto data() { return ptr; }

    private:
        T* ptr = nullptr;
        size_t length = 0u;
    };

}