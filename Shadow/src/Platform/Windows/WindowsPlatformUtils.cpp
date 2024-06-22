#include "sdpch.h"
#include "Shadow/Utils/PlatformUtils.h"

#include "GLFW/glfw3.h"

namespace Shadow
{
    float Time::GetTime()
    {
        return glfwGetTime();
    }
}