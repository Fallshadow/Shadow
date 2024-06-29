#pragma once

#include "Shadow/Core/Window.h"
#include "Shadow/Renderer/GraphicsContext.h"
#include <GLFW/glfw3.h>

namespace Shadow
{
    class WindowsWindow : public Window
    {
    public:
        WindowsWindow(const WindowProps& props);
        virtual ~WindowsWindow();
        virtual void OnUpdate() override;

        virtual unsigned int GetWidth() const override { return m_Data.Width; }
        virtual unsigned int GetHeight() const override { return m_Data.Height; }
        virtual void* GetNativeWindow() const { return m_Window; }

        virtual void SetVSync(bool enabled) override;
        virtual bool IsVSync() const override;

        virtual void SetEventCallback(const EventCallbackFn& callback) override { m_Data.EventCallback = callback; }
    private:
        virtual void Init(const WindowProps& props);
        virtual void Shutdown();
    private:
        GLFWwindow* m_Window;
        Scope<GraphicsContext> m_Context;

        struct WindowData
        {
            std::string Title;
            unsigned int Width, Height;
            bool VSync;

            EventCallbackFn EventCallback;
        };
        WindowData m_Data;
    };
}
