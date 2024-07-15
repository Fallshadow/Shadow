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

    template<typename... Component>
    static void CopyComponent(entt::registry& dst, entt::registry& src, const std::unordered_map<UUID, entt::entity>& enttMap)
    {
        (
            [&]()
            {
                auto view = src.view<Component>();
                for (auto srcEntity : view)
                {
                    entt::entity dstEntity = enttMap.at(src.get<IDComponent>(srcEntity).ID);

                    auto& srcComponent = src.get<Component>(srcEntity);
                    dst.emplace_or_replace<Component>(dstEntity, srcComponent);
                }
            }(),
                ...);
    }

    template<typename... Component>
    static void CopyComponent(ComponentGroup<Component...>, entt::registry& dst, entt::registry& src, const std::unordered_map<UUID, entt::entity>& enttMap)
    {
        CopyComponent<Component...>(dst, src, enttMap);
    }

    template<typename... Component>
    static void CopyComponentIfExists(Entity dst, Entity src)
    {
        (
            [&]()
            {
                if (src.HasComponent<Component>())
                    dst.AddOrReplaceComponent<Component>(src.GetComponent<Component>());
            }(),
                ...);
    }

    template<typename... Component>
    static void CopyComponentIfExists(ComponentGroup<Component...>, Entity dst, Entity src)
    {
        CopyComponentIfExists<Component...>(dst, src);
    }

    Ref<Scene> Scene::Copy(Ref<Scene> other)
    {
        Ref<Scene> newScene = CreateRef<Scene>();

        newScene->m_ViewportWidth = other->m_ViewportWidth;
        newScene->m_ViewportHeight = other->m_ViewportHeight;

        // 在新场景中创建实体
        auto& srcSceneRegistry = other->m_Registry;
        auto& dstSceneRegistry = newScene->m_Registry;
        std::unordered_map<UUID, entt::entity> enttMap;
        auto idView = srcSceneRegistry.view<IDComponent>();
        for (auto e : idView)
        {
            UUID uuid = srcSceneRegistry.get<IDComponent>(e).ID;
            const auto& name = srcSceneRegistry.get<TagComponent>(e).Tag;
            Entity newEntity = newScene->CreateEntityWithUUID(uuid, name);
            enttMap[uuid] = (entt::entity)newEntity;
        }

        CopyComponent(AllComponents{}, dstSceneRegistry, srcSceneRegistry, enttMap);

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


    Entity Scene::DuplicateEntity(Entity entity)
    {
        std::string name = entity.GetName();
        Entity newEntity = CreateEntity(name);
        CopyComponentIfExists(AllComponents{}, newEntity, entity);
        return newEntity;
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

    void Scene::OnRuntimeStart()
    {
        
    }

    void Scene::OnRuntimeStop()
    {
        
    }

    void Scene::OnSimulationStart()
    {

    }

    void Scene::OnSimulationStop()
    {

    }

    void Scene::Step(int frames)
    {
        m_StepFrames = frames;
    }

    void Scene::OnUpdateRuntime(TimeStep ts)
    {
        if (!m_IsPaused || m_StepFrames-- > 0)
        {
            // Update scripts
        }

        // Physics

        // Render 2D
    }

    void Scene::OnUpdateSimulation(TimeStep ts, EditorCamera& camera)
    {
        if (!m_IsPaused || m_StepFrames-- > 0)
        {
            // Physics
        }

        // Render
        RenderScene(camera);
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