#include "SDpch.h"
#include "Platform/OpenGL/OpenGLContext.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace Shadow
{
    OpenGLContext::OpenGLContext(GLFWwindow* windowHandle) : m_WindowHandle(windowHandle)
    {

    }

    void OpenGLContext::Init()
    {
        glfwMakeContextCurrent(m_WindowHandle);
        int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

        SD_CORE_ASSERT(status, "Failed to initialize Glad!")

        SD_CORE_INFO("OpenGL Info:");
        SD_CORE_INFO("  Vendor: {0}", (const char*)glGetString(GL_VENDOR));
        SD_CORE_INFO("  Renderer: {0}", (const char*)glGetString(GL_RENDERER));
        SD_CORE_INFO("  Version: {0}", (const char*)glGetString(GL_VERSION));

        SD_CORE_ASSERT(GLVersion.major > 4 || (GLVersion.major == 4 && GLVersion.minor >= 5), "Hazel requires at least OpenGL version 4.5!");
    }

    void OpenGLContext::SwapBuffers()
    {
        glfwSwapBuffers(m_WindowHandle);
    }
}
