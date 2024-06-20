#pragma once
#include <filesystem>

#include "Shadow/Core/Base.h"
#include "Shadow/Core/Tool/Log.h"

#ifdef SD_ENABLE_ASSERTS

    #if defined(SD_PLATFORM_WINDOWS)
        #define SD_DEBUGBREAK() __debugbreak()
    #else
        #error "Platform doesn't support debugbreak yet!"
    #endif

    // 通过type传入的 _ 或者 _CORE_ 来选择执行 SD_ERROR 还是 SD_CORE_ERROR
    #define SD_INTERNAL_ASSERT_IMPL(type, check, msg, ...) if(!(check)){ SD##type##ERROR(msg, __VA_ARGS__); SD_DEBUGBREAK(); }

    // 外带报错信息，即带两个参数的断言
    #define SD_INTERNAL_ASSERT_WITH_MSG(type, check, ...) SD_INTERNAL_ASSERT_IMPL(type, check, "Assertion failed: {0}", __VA_ARGS__)
    // 自行提示报错信息，输出文件名，报错代码行，即只带一个参数的断言
    #define SD_INTERNAL_ASSERT_NO_MSG(type, check) SD_INTERNAL_ASSERT_IMPL(type, check, "Assertion '{0}' failed at {1}:{2}", SD_STRINGIFY_MACRO(check), std::filesystem::path(__FILE__).filename().string(), __LINE__)
    
    // 保留第三个参数，为了实现一个参数选择 SD_INTERNAL_ASSERT_NO_MSG，两个参数选择 SD_INTERNAL_ASSERT_WITH_MSG
    #define SD_INTERNAL_ASSERT_GET_MACRO_NAME(arg1, arg2, macro, ...) macro
    #define SD_INTERNAL_ASSERT_GET_MACRO(...) SD_EXPAND_MACRO( SD_INTERNAL_ASSERT_GET_MACRO_NAME(__VA_ARGS__, SD_INTERNAL_ASSERT_WITH_MSG, SD_INTERNAL_ASSERT_NO_MSG) )

    // 核心断言和APP断言
    // SD_EXPAND_MACRO是为了防止__VA_ARGS__的展开错误，提前让参数进入到__VA_ARGS__中
    #define SD_ASSERT(...) SD_EXPAND_MACRO( SD_INTERNAL_ASSERT_GET_MACRO(__VA_ARGS__)(_, __VA_ARGS__) )
    #define SD_CORE_ASSERT(...) SD_EXPAND_MACRO( SD_INTERNAL_ASSERT_GET_MACRO(__VA_ARGS__)(_CORE_, __VA_ARGS__) )
#else
    #define SD_ASSERT(...)
    #define SD_CORE_ASSERT(...)
#endif