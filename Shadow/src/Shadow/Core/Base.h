#pragma once
#include "Shadow/Core/Tool/PlatformDetection.h"
#include <memory>

#ifdef SD_DEBUG
    #define SD_ENABLE_ASSERTS
#else
    #define SD_DEBUGBREAK()
#endif

#define SD_EXPAND_MACRO(x) x
#define SD_STRINGIFY_MACRO(x) #x

namespace Shadow
{
    template<typename T>
    using Scope = std::unique_ptr<T>;
    template<typename T, typename ... Args>
    constexpr Scope<T> CreateScope(Args&& ... args)
    {
        return std::make_unique<T>(std::forward<Args>(args)...);
    }

    template<typename T>
    using Ref = std::shared_ptr<T>;
    template<typename T, typename ... Args>
    constexpr Ref<T> CreateRef(Args&& ... args)
    {
        return std::make_shared<T>(std::forward<Args>(args)...);
    }
}