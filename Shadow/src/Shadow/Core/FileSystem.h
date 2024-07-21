#pragma once

#include "Shadow/Core/Buffer.h"

namespace Shadow
{
    class FileSystem
    {
    public:
        // TODO: move to FileSystem class
        static Buffer ReadFileBinary(const std::filesystem::path& filepath);
    };
}