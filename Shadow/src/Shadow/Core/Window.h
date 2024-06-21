#pragma once

#include "sdpch.h"
#include "Shadow/Core/Base.h"
#include "Shadow/Events/Event.h"

namespace Shadow
{
    // 窗口属性数据（初始化用，各个平台窗口都应该有的属性）
    struct WindowProps
    {
        std::string Title;
        uint32_t Width, Height;

        // 窗口默认参数
        WindowProps(const std::string& title = "Shadow Engine", uint32_t width = 1600, uint32_t height = 900)
            : Title(title), Width(width), Height(height) { }
    };

    // 抽象类 需要在每个平台上实现
    class Window
    {
    public:
        static Scope<Window> Create(const WindowProps& props = WindowProps());
        virtual ~Window() = default;
        virtual void OnUpdate() = 0;

        virtual uint32_t GetWidth() const = 0;
        virtual uint32_t GetHeight() const = 0;
        virtual void* GetNativeWindow() const = 0;

        virtual void SetVSync(bool enabled) = 0;
        virtual bool IsVSync() const = 0;

        using EventCallbackFn = std::function<void(Event&)>;
        virtual void SetEventCallback(const EventCallbackFn& callback) = 0;
    };
}
