#include "Core\conv.h"

#include <vector>
#include <sstream>

// for mbstowcs
#pragma warning (disable : 4996)

namespace core::string
{
    namespace helper
    {
        std::vector<int> to_int_list(std::string_view input)
        {
            std::vector<int> output;
            std::stringstream stream(input.data());

            std::string string;
            while (std::getline(stream, string, ',')) output.emplace_back(std::stoi(string));

            return output;
        }

        std::vector<float> to_float_list(std::string_view input)
        {
            std::vector<float> output;
            std::stringstream stream(input.data());

            std::string string;
            while (std::getline(stream, string, ',')) output.emplace_back(std::stof(string));

            return output;
        }
    }

    int to_integer(std::string_view value)
    {
        return std::stoi(value.data());
    }

    ivec2 to_ivec2(std::string_view value)
    {
        auto ints = helper::to_int_list(value);
        if (ints.size() != 2) return ivec2(0);

        return ivec2(ints.at(0), ints.at(1));
    }

    ivec3 to_ivec3(std::string_view value)
    {
        auto ints = helper::to_int_list(value);
        if (ints.size() != 3) return ivec3(0);

        return ivec3(ints.at(0), ints.at(1), ints.at(2));
    }

    ivec4 to_ivec4(std::string_view value)
    {
        auto ints = helper::to_int_list(value);
        if (ints.size() != 4) return ivec4(0);

        return ivec4(ints.at(0), ints.at(1), ints.at(2), ints.at(3));
    }
    
    u64 to_uint64(std::string_view value)
    {
        return (u64)std::stoull(value.data());
    }

    float to_float(std::string_view value)
    {
        return std::stof(value.data());
    }

    vec2 to_vec2(std::string_view value)
    {
        auto floats = helper::to_float_list(value);
        if (floats.size() != 2) return vec2(0.0f);

        return vec2(floats.at(0), floats.at(1));
    }

    vec3 to_vec3(std::string_view value)
    {
        auto floats = helper::to_float_list(value);
        if (floats.size() != 3) return vec3(0.0f);

        return vec3(floats.at(0), floats.at(1), floats.at(2));
    }

    vec4 to_vec4(std::string_view value)
    {
        auto floats = helper::to_float_list(value);
        if (floats.size() != 4) return vec4(0.0f);

        return vec4(floats.at(0), floats.at(1), floats.at(2), floats.at(3));
    }

    std::wstring to_wstring(std::string_view value)
    {
        std::wstring result(value.length(), L'#');
        mbstowcs(result.data(), value.data(), result.length());
        return result;
    }

    std::string from_ivec2(ivec2 const& value)
    {
        std::stringstream stream;
        stream << value.x << "," << value.y;

        return stream.str();
    }
}