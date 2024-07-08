#pragma once

#include <string>

namespace Shadow
{
    class Time
    {
    public:
        static float GetTime();
    };

    class FileDialogs
    {
    public:
        // 如果失败返回空字符
        static std::string OpenFile(const char* filter);
        static std::string SaveFile(const char* filter);
    };
}
