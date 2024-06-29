#include "UT1FocusRendererLayer.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace Shadow
{
    UT1FocusRendererLayer::UT1FocusRendererLayer() : Layer("UT1FocusRendererLayer"), m_Camera(-m_AspectRatio * m_ZoomLevel, m_AspectRatio* m_ZoomLevel, -m_ZoomLevel, m_ZoomLevel)
    {
        m_SquareVA = Shadow::VertexArray::Create();
        float squareVertices[5 * 4] = {
            -0.5f, -0.5f, 0.0f, 0.0f, 0.0f,
             0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
             0.5f,  0.5f, 0.0f, 1.0f, 1.0f,
            -0.5f,  0.5f, 0.0f, 0.0f, 1.0f
        };

        Shadow::Ref<Shadow::VertexBuffer> squareVB = Shadow::VertexBuffer::Create(squareVertices, sizeof(squareVertices));
        squareVB->SetLayout({
            {"a_Position", Shadow::ShaderDataType::Float3},
            {"a_TexCoord", Shadow::ShaderDataType::Float2}
            });
        m_SquareVA->AddVertexBuffer(squareVB);

        unsigned int squareIndices[6] = { 0, 1, 2, 2, 3, 0 };
        Shadow::Ref<Shadow::IndexBuffer> squareIB = Shadow::IndexBuffer::Create(squareIndices, sizeof(squareIndices) / sizeof(uint32_t));
        m_SquareVA->SetIndexBuffer(squareIB);

        std::string flatColorShaderVertexSrc = R"(
			#version 330 core
			layout(location = 0) in vec3 a_Position;

			uniform mat4 u_ViewProjection;
			uniform mat4 u_Transform;

			void main()
			{
				gl_Position = u_ViewProjection * u_Transform * vec4(a_Position, 1.0);
			}
		)";

        std::string flatColorShaderFragmentSrc = R"(
			#version 330 core

			out vec4 color;

			uniform vec3 u_Color;

			void main()
			{
				color = vec4(u_Color, 1.0);
			}
		)";

        m_FlatColorShader = Shadow::Shader::Create("FlatColor", flatColorShaderVertexSrc, flatColorShaderFragmentSrc);

        auto textureShader = m_ShaderLibrary.Load("assets/shaders/Texture.glsl");

        m_Texture = Shadow::Texture2D::Create("assets/textures/Checkerboard.png");
        m_ChernoLogoTexture = Shadow::Texture2D::Create("assets/textures/ChernoLogo.png");

        textureShader->Bind();
        textureShader->SetInt("u_Texture", 0);
    }

    void UT1FocusRendererLayer::OnAttach()
    {

    }

    void UT1FocusRendererLayer::OnDetach()
    {

    }

    void UT1FocusRendererLayer::OnUpdate(TimeStep ts)
    {
        Shadow::RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1 });
        Shadow::RenderCommand::Clear();

        Shadow::Renderer::BeginScene(m_Camera);

        glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(0.1f));

        m_FlatColorShader->Bind();
        m_FlatColorShader->SetFloat3("u_Color", m_SquareColor);

        for (int y = 0; y < 5; y++)
        {
            for (int x = 0; x < 5; x++)
            {
                glm::vec3 pos(x * 0.11f, y * 0.11f, 0.0f);
                glm::mat4 transform = glm::translate(glm::mat4(1.0f), pos) * scale;
                Shadow::Renderer::Submit(m_FlatColorShader, m_SquareVA, transform);
            }
        }

        auto textureShader = m_ShaderLibrary.Get("Texture");

        m_ChernoLogoTexture->Bind();
        Shadow::Renderer::Submit(textureShader, m_SquareVA, glm::scale(glm::mat4(1.0f), glm::vec3(1.5f)));
        m_Texture->Bind();
        Shadow::Renderer::Submit(textureShader, m_SquareVA, glm::scale(glm::mat4(1.0f), glm::vec3(1.5f)));

        Shadow::Renderer::EndScene();
    }

    void UT1FocusRendererLayer::OnImGuiRender()
    {
    }

    void UT1FocusRendererLayer::OnEvent(Event& e)
    {

    }
}