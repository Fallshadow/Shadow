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

    Entity Scene::CreateEntity(const std::string& name)
    {
        return CreateEntityWithUUID(UUID(), name);
    }

    Entity Scene::CreateEntityWithUUID(UUID uuid, const std::string& name)
    {
        Entity entity = { m_Registry.create(), this };
        entity.AddComponent<IDComponent>(uuid);
        entity.AddComponent<TransformComponent>();
        auto& tag = entity.AddComponent<TagComponent>();
        tag.Tag = name.empty() ? "Entity" : name;

        m_EntityMap[uuid] = entity;

        return entity;
    }

    void Scene::DestroyEntity(Entity entity)
    {
        m_EntityMap.erase(entity.GetUUID());
        m_Registry.destroy(entity);
    }

    void Scene::RenderScene(EditorCamera& camera)
    {
        Renderer2D::BeginScene(camera);

        // Draw sprites
        {
            auto group = m_Registry.group<TransformComponent>(entt::get<SpriteRendererComponent>);
            for (auto entity : group)
            {
                auto [transform, sprite] = group.get<TransformComponent, SpriteRendererComponent>(entity);

                if (sprite.Texture)
                {
                    Renderer2D::DrawQuad(transform.GetTransform(), sprite.Texture, sprite.TilingFactor, sprite.Color, (int)entity);
                }
                else
                {
                    Renderer2D::DrawQuad(transform.GetTransform(), sprite.Color, (int)entity);
                }
            }
        }

        // Draw circles
        {
            auto view = m_Registry.view<TransformComponent, CircleRendererComponent>();
            for (auto entity : view)
            {
                auto [transform, circle] = view.get<TransformComponent, CircleRendererComponent>(entity);

                Renderer2D::DrawCircle(transform.GetTransform(), circle.Color, circle.Thickness, circle.Fade, (int)entity);
            }

            Renderer2D::EndScene();
        }
    }

    // 附加/替换 组件时有些组件需要进行一些操作
    template<typename T>
    void Scene::OnComponentAdded(Entity entity, T& component)
    {
        static_assert(sizeof(T) == 0);
    }

    template<> void Scene::OnComponentAdded<IDComponent>(Entity entity, IDComponent& component) { }
    template<> void Scene::OnComponentAdded<TransformComponent>(Entity entity, TransformComponent& component) { }
    template<> void Scene::OnComponentAdded<SpriteRendererComponent>(Entity entity, SpriteRendererComponent& component) { }
    template<> void Scene::OnComponentAdded<CircleRendererComponent>(Entity entity, CircleRendererComponent& component) { }
    template<> void Scene::OnComponentAdded<TagComponent>(Entity entity, TagComponent& component) { }
}