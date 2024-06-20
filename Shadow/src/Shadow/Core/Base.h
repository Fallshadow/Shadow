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

#define BIT(x) (1 << x)
// 创建一个函数对象，该对象将成员函数绑定到当前对象上，以便在事件处理中使用
// 不定参数类型数量，推断参数完美转发，推断返回类型
#define SD_BIND_EVENT_FN(fn) [this](auto&& ... args) -> decltype(auto) { return this->fn(std::forward<decltype(args)>(args)...); }

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