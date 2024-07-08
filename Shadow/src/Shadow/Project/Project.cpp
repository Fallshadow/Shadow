#include "sdpch.h"
#include "Project.h"

#include "ProjectSerializer.h"

namespace Shadow
{
    Ref<Project> Project::New()
    {
        s_ActiveProject = CreateRef<Project>();
        return s_ActiveProject;
    }

    Ref<Project> Project::Load(const std::filesystem::path& path)
    {
        Ref<Project> project = CreateRef<Project>();

        ProjectSerializer ps(project);
        if (ps.Deserialize(path))
        {
            project->m_ProjectDirectory = path.parent_path();
            s_ActiveProject = project;
            return s_ActiveProject;
        }

        return nullptr;
    }

    bool Project::Save(const std::filesystem::path& path)
    {
        ProjectSerializer ps(s_ActiveProject);
        if (ps.Serialize(path))
        {
            s_ActiveProject->m_ProjectDirectory = path.parent_path();
            return true;
        }

        return false;
    }
}
