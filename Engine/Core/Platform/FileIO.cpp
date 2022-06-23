#include "Platform\FileIO.h"

#include <vector>

StreamReader::~StreamReader()
{
    if (file.is_open())
        file.close();
}

void StreamReader::read(void* data, size_t size)
{
    file.read(reinterpret_cast<char*>(data), size);
}

StreamWriter::~StreamWriter()
{
    if (file.is_open())
        file.close();
}

void StreamWriter::write(void const* data, size_t size)
{
    file.write(reinterpret_cast<char const*>(data), size);
}

namespace core
{
    StreamReader::Ref OpenStreamReader(std::string_view path)
    {
        auto reader = std::make_shared<StreamReader>();
        reader->file.open(path.data(), std::ios::binary);
        return reader;
    }

    void CloseStreamReader(StreamReader::Ref reader)
    {
        if (reader->file.is_open())
            reader->file.close();
    }
    
    StreamWriter::Ref OpenStreamWriter(std::string_view path)
    {
        auto writer = std::make_shared<StreamWriter>();
        writer->file.open(path.data(), std::ios::binary);
        return writer;
    }
    
    void CloseStreamWriter(StreamWriter::Ref writer)
    {
        if (writer->file.is_open())
            writer->file.close();
    }

    namespace fileio
    {
        std::vector<char> LoadBinaryFile(std::string_view path)
        {
            std::ifstream stream(path.data(), std::ifstream::ate | std::ifstream::binary);

            if (stream.is_open())
            {
                std::vector<char> buffer(stream.tellg());
                stream.seekg(0);
                stream.read(buffer.data(), buffer.size());
                
                stream.close();
                
                return buffer;
            }

            return {};
        }

        std::string LoadFileToString(std::string_view path)
        {
            std::ifstream file(path.data());
            std::vector<char> buffer((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

            auto result = std::string(buffer.begin(), buffer.end());
            file.close();

            return result;
        }

    }
}