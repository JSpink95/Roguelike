#pragma once
#include "Core.h"
#include "Core\Object.h"

#include <vector>
#include <fstream>
#include <Windows.h>

struct StreamReader
    : public Object
{
    BIND_OBJECT(StreamReader);

    ~StreamReader();
    void read(void* data, size_t size);

    std::ifstream file;
};

struct StreamWriter
    : public Object
{
    BIND_OBJECT(StreamWriter);

    ~StreamWriter();
    void write(void const* data, size_t size);

    std::ofstream file;
};

namespace core
{
    StreamReader::Ref OpenStreamReader(std::string_view path);
    void CloseStreamReader(StreamReader::Ref reader);

    StreamWriter::Ref OpenStreamWriter(std::string_view path);
    void CloseStreamWriter(StreamWriter::Ref writer);

    namespace fileio
    {
        std::vector<char> LoadBinaryFile(std::string_view path);
        std::string LoadFileToString(std::string_view path);
    }
}
