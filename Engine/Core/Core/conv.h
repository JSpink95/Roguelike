#pragma once
#include "Core.h"

namespace core::string
{
    // ints
    int to_integer(std::string_view value);
    ivec2 to_ivec2(std::string_view value);
    ivec3 to_ivec3(std::string_view value);
    ivec4 to_ivec4(std::string_view value);

    u64 to_uint64(std::string_view value);
    
    // floats
    float to_float(std::string_view value);
    vec2 to_vec2(std::string_view value);
    vec3 to_vec3(std::string_view value);
    vec4 to_vec4(std::string_view value);

    // strings
    std::wstring to_wstring(std::string_view value);

    std::string from_ivec2(ivec2 const& value);

    template<typename T>
    T convert(std::string_view value) = delete;

    template<typename T>
    std::string convert(T const&) = delete;

    template<> inline auto convert<int>(std::string_view value)             -> int          { return to_integer(value); }
    template<> inline auto convert<u64>(std::string_view value)             -> u64          { return to_uint64(value); }
    template<> inline auto convert<ivec2>(std::string_view value)           -> ivec2        { return to_ivec2(value); }
    template<> inline auto convert<ivec3>(std::string_view value)           -> ivec3        { return to_ivec3(value); }
    template<> inline auto convert<ivec4>(std::string_view value)           -> ivec4        { return to_ivec4(value); }
    template<> inline auto convert<float>(std::string_view value)           -> float        { return to_float(value); }
    template<> inline auto convert<vec2>(std::string_view value)            -> vec2         { return to_vec2(value); }
    template<> inline auto convert<vec3>(std::string_view value)            -> vec3         { return to_vec3(value); }
    template<> inline auto convert<vec4>(std::string_view value)            -> vec4         { return to_vec4(value); }
    template<> inline auto convert<std::wstring>(std::string_view value)    -> std::wstring { return to_wstring(value); }

    template<> inline std::string convert<ivec2>(ivec2 const& value) { return from_ivec2(value); }
}
