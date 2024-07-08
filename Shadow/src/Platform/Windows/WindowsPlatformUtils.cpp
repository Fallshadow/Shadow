#include "sdpch.h"
#include "Shadow/Utils/PlatformUtils.h"
#include "Shadow/Core/Application.h"

// Windows API 中的一个头文件，提供了对公共对话框功能的访问，如打开和保存文件对话框等。
#include <commdlg.h>
#include <GLFW/glfw3.h>
// 暴露本地 Windows API 扩展所需的头文件。
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>

namespace Shadow
{
    float Time::GetTime()
    {
        return glfwGetTime();
    }

    std::string FileDialogs::OpenFile(const char* filter)
    {
        OPENFILENAMEA ofn;
        CHAR szFile[260] = { 0 };
        CHAR currentDir[256] = { 0 };
        ZeroMemory(&ofn, sizeof(OPENFILENAME));
        ofn.lStructSize = sizeof(OPENFILENAME);
        ofn.hwndOwner = glfwGetWin32Window((GLFWwindow*)Application::Get().GetWindow().GetNativeWindow());
        ofn.lpstrFile = szFile;
        ofn.nMaxFile = sizeof(szFile);
        if (GetCurrentDirectoryA(256, currentDir))
            ofn.lpstrInitialDir = currentDir;
        ofn.lpstrFilter = filter;
        ofn.nFilterIndex = 1;
        ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;
        if (GetOpenFileNameA(&ofn) == TRUE)
        {
            return ofn.lpstrFile;
        }
        return std::string();
    }

    std::string FileDialogs::SaveFile(const char* filter)
    {
        OPENFILENAMEA ofn;
        CHAR szFile[260] = { 0 };
        CHAR currentDir[256] = { 0 };
        // 用于将指定内存区域的所有字节清零。
        ZeroMemory(&ofn, sizeof(OPENFILENAME));
        // 设置大小
        ofn.lStructSize = sizeof(OPENFILENAME);
        // 父窗口句柄，文件对话框的拥有者窗口
        ofn.hwndOwner = glfwGetWin32Window((GLFWwindow*)Application::Get().GetWindow().GetNativeWindow());
        // 用于接收用户选择的文件路径
        ofn.lpstrFile = szFile;
        // 接收文件路径的缓冲区大小
        ofn.nMaxFile = sizeof(szFile);
        // 该函数的作用是将当前工作目录的路径复制到指定的缓冲区中，并返回实际复制的字符数（不包括终止 null 字符）。
        // 如果函数成功，返回值是复制到缓冲区的字符数；如果函数失败，返回值为 0。
        if (GetCurrentDirectoryA(256, currentDir))
            // 初始目录
            ofn.lpstrInitialDir = currentDir;
        // 文件过滤器，指定允许选择的文件类型
        ofn.lpstrFilter = filter;
        // 默认选中的过滤器索引
        ofn.nFilterIndex = 1;
        // 对话框标志，控制对话框的外观和行为
        // OFN_FILEMUSTEXIST：指定用户只能选择已经存在的文件。
        // OFN_PATHMUSTEXIST：指定用户只能选择已经存在的路径。
        // OFN_OVERWRITEPROMPT：如果用户选择了一个已经存在的文件，会弹出一个提示框询问是否覆盖。
        // OFN_ALLOWMULTISELECT：允许用户选择多个文件。
        // OFN_EXPLORER：使用新版的文件对话框样式。
        // OFN_NOCHANGEDIR：无论用户选择的文件是在哪个目录，程序运行时的当前工作目录都不会被改变。这有助于程序在特定情况下保持对当前目录的控制，而不受用户选择文件对当前目录的影响。
        ofn.Flags = OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT | OFN_NOCHANGEDIR;
        // 默认文件扩展名
        ofn.lpstrDefExt = std::strchr(filter, '\0') + 1;

        // 用于显示一个标准的文件保存对话框，允许用户选择文件保存的路径和名称
        // lpofn：指向一个 OPENFILENAMEA 结构体的指针，用于指定文件对话框的外观和行为。
        if (GetSaveFileNameA(&ofn) == TRUE)
        {
            return ofn.lpstrFile;
        }

        return std::string();
    }
}