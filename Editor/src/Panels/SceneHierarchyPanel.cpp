#include "SceneHierarchyPanel.h"

#include <imgui.h>
#include <imgui_internal.h>
#include <misc/cpp/imgui_stdlib.h>

#include <glm/gtc/type_ptr.hpp>

#include <cstring>

namespace Shadow
{
    SceneHierarchyPanel::SceneHierarchyPanel(const Ref<Scene>& context)
    {
        SetScene(context);
    }

    void SceneHierarchyPanel::SetScene(const Ref<Scene>& context)
    {
        m_Scene = context;
    }

    void SceneHierarchyPanel::OnImGuiRender()
    {
        ImGui::Begin("Scene Hierarchy");

        ImGui::End();

        ImGui::Begin("Properties");

        ImGui::End();
    }
}