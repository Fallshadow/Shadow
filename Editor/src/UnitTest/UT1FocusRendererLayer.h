#pragma once

#include "Shadow.h"

namespace Shadow
{

    class UT1FocusRendererLayer : public Layer
    {
    public:
        UT1FocusRendererLayer();
        virtual ~UT1FocusRendererLayer() = default;

        virtual void OnAttach() override;
        virtual void OnDetach() override;

        void OnUpdate(TimeStep ts) override;
        virtual void OnImGuiRender() override;
        void OnEvent(Event& e) override;

    private:
        float m_AspectRatio = 1280.0f / 720.0f, m_ZoomLevel = 1.0f;
        OrthographicCamera m_Camera;

        Shadow::Ref<Shadow::VertexArray> m_SquareVA;

        Shadow::Ref<Shadow::Texture2D> m_Texture, m_ChernoLogoTexture;
        Shadow::ShaderLibrary m_ShaderLibrary;
        Shadow::Ref<Shadow::Shader> m_FlatColorShader;

        glm::vec3 m_SquareColor = { 0.2f, 0.3f, 0.8f };
    };
}