#pragma once

#include "Shadow.h"

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

        glm::vec2 m_ViewportBounds[2];
        bool m_ViewportHovered = false;
    };
}