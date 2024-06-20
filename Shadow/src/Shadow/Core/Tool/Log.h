#pragma once
#include "Shadow/Core/Base.h"

// 这将忽略在外部标头中引发的所有警告
#pragma warning(push, 0)
#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>
#pragma warning(pop)

namespace Shadow
{
    class Log
    {
    public:
        static void Init();

        static Ref<spdlog::logger>& GetCoreLogger() { return s_CoreLogger; }
        static Ref<spdlog::logger>& GetClientLogger() { return s_ClientLogger; }
    private:
        static Ref<spdlog::logger> s_CoreLogger;
        static Ref<spdlog::logger> s_ClientLogger;
    };
}


// Core log macros
#define SD_CORE_TRACE(...)      ::Shadow::Log::GetCoreLogger()->trace(__VA_ARGS__)
#define SD_CORE_INFO(...)       ::Shadow::Log::GetCoreLogger()->info(__VA_ARGS__)
#define SD_CORE_WARN(...)       ::Shadow::Log::GetCoreLogger()->warn(__VA_ARGS__)
#define SD_CORE_ERROR(...)      ::Shadow::Log::GetCoreLogger()->error(__VA_ARGS__)
#define SD_CORE_CRITICAL(...)   ::Shadow::Log::GetCoreLogger()->critical(__VA_ARGS__)

// Client log macros
#define SD_TRACE(...)           ::Shadow::Log::GetClientLogger()->trace(__VA_ARGS__)
#define SD_INFO(...)            ::Shadow::Log::GetClientLogger()->info(__VA_ARGS__)
#define SD_WARN(...)            ::Shadow::Log::GetClientLogger()->warn(__VA_ARGS__)
#define SD_ERROR(...)           ::Shadow::Log::GetClientLogger()->error(__VA_ARGS__)
#define SD_CRITICAL(...)        ::Shadow::Log::GetClientLogger()->critical(__VA_ARGS__)