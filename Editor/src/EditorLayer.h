#pragma once

#include "Shadow.h"
#include "Panels/SceneHierarchyPanel.h"
#include "Panels/ContentBrowserPanel.h"

namespace Shadow
{
    class EditorLayer : public Layer
    {
    public:
        EditorLayer();
        virtual ~EditorLayer() = default;

        virtual void OnAttach() override;
        virtual void OnDetach() override;

        void OnUpdate(TimeStep ts) override;
        virtual void OnImGuiRender() override;
        void OnEvent(Event& e) override;

        bool OpenProject();
        void OpenProject(const std::filesystem::path& path);
        void OpenScene(const std::filesystem::path& path);
        void NewScene();
        void SaveScene();
        void SaveSceneAs();

    private:
        void SerializeScene(Ref<Scene> scene, const std::filesystem::path& path);

        void UI_Toolbar();
        void OnScenePlay();
        void OnSceneSimulate();
        void OnSceneStop();
        void OnScenePause();
        // 覆盖层渲染：选中物体边框
        void OnOverlayRender();

        bool OnKeyPressed(KeyPressedEvent& e);
        bool OnMouseButtonPressed(MouseButtonPressedEvent& e);

        void OnDuplicateEntity();
    private:
        glm::vec2 m_ViewportSize = { 0.0f, 0.0f };
        Ref<FrameBuffer> m_FrameBuffer;

        enum class SceneState
        {
            Edit = 0, Play = 1, Simulate = 2
        };
        SceneState m_SceneState = SceneState::Edit;
        Ref<Scene> m_ActiveScene;
        Ref<Scene> m_EditorScene;
        EditorCamera m_EditorCamera;

        // ViewPortPanel
        glm::vec2 m_ViewportBounds[2];
        bool m_ViewportHovered = false;
        Entity m_HoveredEntity;

        // ScenePanel
        SceneHierarchyPanel m_SceneHierarchyPanel;
        std::filesystem::path m_EditorScenePath;

        // Panels
        Scope<ContentBrowserPanel> m_ContentBrowserPanel;

        // ToolBar
        Ref<Texture2D> m_IconPlay, m_IconPause, m_IconStep, m_IconSimulate, m_IconStop;

        // Gizmo
        int m_GizmoType = -1;
    };
}