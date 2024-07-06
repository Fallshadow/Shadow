#include "sdpch.h"
#include "Scene.h"
#include "Shadow/Renderer/Renderer2D.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

namespace Shadow
{
    Scene::Scene() { }

    Scene::~Scene() { }

    Ref<Scene> Scene::Copy(Ref<Scene> other)
    {
        Ref<Scene> newScene = CreateRef<Scene>();

        newScene->m_ViewportWidth = other->m_ViewportWidth;
        newScene->m_ViewportHeight = other->m_ViewportHeight;

        return newScene;
    }

    void Scene::OnUpdateEditor(EditorCamera& camera)
    {
        // Render
        RenderScene(camera);
    }

    void Scene::OnViewportResize(uint32_t width, uint32_t height)
    {
        if (m_ViewportWidth == width && m_ViewportHeight == height)
            return;

        m_ViewportWidth = width;
        m_ViewportHeight = height;
    }

    void Scene::RenderScene(EditorCamera& camera)
    {
        Renderer2D::BeginScene(camera);

        glm::vec3 QuadTranslation = { 0.0f, 0.0f, 0.0f };
        glm::vec3 CircleTranslation = { 0.5f, 0.5f, 0.0f };
        glm::vec3 Rotation = { 0.0f, 0.0f, 0.0f };
        glm::vec3 Scale = { 1.0f, 1.0f, 1.0f };
        glm::mat4 rotation = glm::toMat4(glm::quat(Rotation));

        glm::mat4 QuadTransform = glm::translate(glm::mat4(1.0f), QuadTranslation) * rotation * glm::scale(glm::mat4(1.0f), Scale);
        glm::mat4 CircleTransform = glm::translate(glm::mat4(1.0f), CircleTranslation) * rotation * glm::scale(glm::mat4(1.0f), Scale);

        Renderer2D::DrawQuad(QuadTransform, glm::vec4(1, 1, 1, 1), 0);
        Renderer2D::DrawCircle(CircleTransform, glm::vec4(0, 0, 0, 1));
        Renderer2D::DrawLine(glm::vec3(-1, -1, 0), glm::vec3(1, 1, 0), glm::vec4(0.5f, 0.5f, 0.5f, 1.0f));

        Renderer2D::EndScene();
    }
}
