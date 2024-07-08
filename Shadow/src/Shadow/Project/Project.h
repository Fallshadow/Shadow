#pragma once

#include <string>
#include <filesystem>

#include "Shadow/Core/Tool/Assert.h"

namespace Shadow 
{
    struct ProjectConfig
    {
        std::string Name = "Untitled";

        std::filesystem::path StartScene;

        std::filesystem::path AssetDirectory;
    };

    // TODO：新建项目的交互窗口
    class Project
    {
    public:
        static Ref<Project> New();
        static Ref<Project> Load(const std::filesystem::path& path);
        static bool Save(const std::filesystem::path& path);


        ProjectConfig& GetConfig() { return m_Config; }
        static Ref<Project> GetActive() { return s_ActiveProject; }

        static const std::filesystem::path& GetProjectDirectory()
        {
            SD_CORE_ASSERT(s_ActiveProject);
            return s_ActiveProject->m_ProjectDirectory;
        }

        static std::filesystem::path GetAssetDirectory()
        {
            SD_CORE_ASSERT(s_ActiveProject);
            return GetProjectDirectory() / s_ActiveProject->m_Config.AssetDirectory;
        }

        // TODO(Yan): move to asset manager when we have one
        static std::filesystem::path GetAssetFileSystemPath(const std::filesystem::path& path)
        {
            SD_CORE_ASSERT(s_ActiveProject);
            return GetAssetDirectory() / path;
        }

    private:
        inline static Ref<Project> s_ActiveProject;

        ProjectConfig m_Config;
        std::filesystem::path m_ProjectDirectory;
    };
}
