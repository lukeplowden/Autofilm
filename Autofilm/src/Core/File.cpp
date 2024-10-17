#include "autofilmpch.h"
#include "Core/File.h"

namespace Autofilm
{
    namespace fs = std::filesystem;
    File::File(const fs::path& path)
        : _path(path)
    {
        bool exists = fs::exists(path);
        if (!exists) {
            AF_CORE_ERROR("Couldn't find a file at {0}", path.string());
        }
        bool isRegular = fs::is_regular_file(path);
        if (!isRegular) {
            AF_CORE_ERROR("{0} is not a regular file.", path.string());
        }
    }

    File::~File()
    {
    }

    std::vector<char> File::read()
    {
        std::ifstream file(_path, std::ios::ate | std::ios::binary);

        if (!file.is_open()) {
            AF_CORE_ERROR("Failed to open file {0}", _path.string());
        }

        size_t fileSize = (size_t) file.tellg();
        std::vector<char> buffer(fileSize);
        file.seekg(0);
        file.read(buffer.data(), fileSize);;
        file.close();

        return buffer;
    }

    std::vector<char> File::readFile(const std::filesystem::path& path)
    {
        std::ifstream file(path, std::ios::ate | std::ios::binary);

        if (!file.is_open()) {
            AF_CORE_ERROR("Failed to open file {0}", path.string());
        }

        size_t fileSize = (size_t) file.tellg();
        std::vector<char> buffer(fileSize);
        file.seekg(0);
        file.read(buffer.data(), fileSize);;
        file.close();

        return buffer;
    }
}