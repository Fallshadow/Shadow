#pragma once

#include "Shadow/Core/Base.h"
#include "Shadow/Scene/Scene.h"

namespace Shadow
{
    class SceneHierarchyPanel
    {
    public:
        SceneHierarchyPanel() = default;
        SceneHierarchyPanel(const Ref<Scene>& context);

        void SetScene(const Ref<Scene>& context);

        void OnImGuiRender();
    private:
        Ref<Scene> m_Scene;
    };
}