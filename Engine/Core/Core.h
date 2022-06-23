#pragma once

#include <any>
#include <string>
#include <memory>
#include <cstdint>

#include <glm\glm.hpp>
#include <glm\gtc\type_ptr.hpp>

using i64 = std::int64_t;

using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using u64 = std::uint64_t;

using vec2 = glm::vec2;
using vec3 = glm::vec3;
using vec4 = glm::vec4;
using mat4 = glm::mat4;

using ivec2 = glm::ivec2;
using ivec3 = glm::ivec3;
using ivec4 = glm::ivec4;

using quat = glm::quat;

// define here to remove SDLs macro for main
#define SDL_MAIN_HANDLED

// graphics platform macros
// just for easy disabling

#define _OPENGL
//#define _VULKAN

// string manip, best kept here because then everything gets it.

#define STRINGIFY2(name) #name
#define STRINGIFY(name) STRINGIFY2(name)

// bitfield stuff
#define BITFIELD_BEGIN(bname)\
namespace bname {

#define BITFIELD_VALUE(bname, index)    \
constexpr u64 bname = (u64)(1 << index)

#define BITFIELD_END() }

#define BITFIELD_CHECK(value, bit) ((value & bit) == bit)

// simple utilities, might get rid/replace with templatised range functions
#define FOR_RANGE_UPTO(var, lower, upper) for (auto var = lower; var < upper; ++var)
#define FOR_RANGE_DOWNTO(var, upper, lower) for (auto var = (upper - 1); var >= lower; --var)

// asset constants
constexpr u64 MAX_ASSET_HEADER_NAME = 32u;
constexpr u64 MAX_ASSET_PATH = 260u;

#define ASSET_FWD_DECL(strukt) struct strukt ## Asset
#define BIND_ASSET(strukt) using AssetType = strukt ## Asset
#define ASSET_STRUCT(strukt) __declspec(align(16)) struct strukt ## Asset

template<typename To, typename From>
static inline To* StaticCast(From* input)
{
    return (input == nullptr) ? nullptr : static_cast<To*>(input);
}

template<typename To, typename From>
static inline To* DynamicCast(From* input)
{
    return (input == nullptr) ? nullptr : dynamic_cast<To*>(input);
}

template<typename To, typename From>
static inline To* ReCast(From* input)
{
    return (input == nullptr) ? nullptr : reinterpret_cast<To*>(input);
}

#ifdef _FINAL

#define LOG(...)
#define LOG_ERROR(...)
#define LOG_WARNING(...)

#else

#define LOG(fmt, ...)\
{\
    SetConsoleTextAttribute( GetStdHandle(STD_OUTPUT_HANDLE), 0x0F);\
    printf(fmt"\n", __VA_ARGS__);\
}

#define LOG_ERROR(fmt, ...)\
{\
    SetConsoleTextAttribute( GetStdHandle(STD_OUTPUT_HANDLE), 0x04);\
    printf(fmt"\n", __VA_ARGS__);\
}

#define LOG_WARNING(fmt, ...)\
{\
    SetConsoleTextAttribute( GetStdHandle(STD_OUTPUT_HANDLE), 0x0E);\
    printf(fmt"\n", __VA_ARGS__);\
}
#endif