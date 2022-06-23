#pragma once
#include <Core.h>
#include <Windows.h>

using asset_id = size_t;

__declspec(align(16)) struct AssetHeader
{
    char asset_type_name[MAX_ASSET_HEADER_NAME];
    size_t version = 1u;
    size_t asset_data_size = 0u;

    AssetHeader()
    {
        ZeroMemory(asset_type_name, MAX_ASSET_HEADER_NAME);
    }
};

static inline asset_id asset_hash(std::string_view name)
{
    static std::hash<std::string_view> hasher;
    return hasher(name);
}