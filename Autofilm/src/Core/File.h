#pragma once

#include "Core/Core.h"
#include <filesystem>
#include <fstream>

namespace Autofilm
{
    class File
    {
    public:
        File(const std::filesystem::path& path);
        ~File();

        std::vector<char> read();
        static std::vector<char> readFile(const std::filesystem::path& path);
    private:
        std::filesystem::path _path;
    };
}