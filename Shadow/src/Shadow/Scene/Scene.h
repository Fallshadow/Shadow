#pragma once

#include "Shadow/Renderer/EditorCamera.h"
#include "Shadow/Core/UUID.h"
#include "Shadow/Scene/Entity.h"

#include <entt.hpp>

// b2World 是 Box2D 物理引擎中的一个重要类，用于表示整个物理世界。
// 在 Box2D 中，b2World 负责管理所有物理实体（如刚体、关节等）、执行物理仿真计算以及处理碰撞检测等功能。
class b2World;

namespace Shadow
{
    class Entity;

    class Scene
    {
    public:
        Scene();
        ~Scene();
        static Ref<Scene> Copy(Ref<Scene> other);
        
        void OnUpdateEditor(EditorCamera& camera);
        void OnViewportResize(uint32_t width, uint32_t height);

        Entity CreateEntity(const std::string& name = std::string());
        Entity CreateEntityWithUUID(UUID uuid, const std::string& name = std::string());
        void DestroyEntity(Entity entity);
        Entity DuplicateEntity(Entity entity);

        Entity GetPrimaryCameraEntity();

        void OnRuntimeStart();
        void OnRuntimeStop();

        void OnSimulationStart();
        void OnSimulationStop();

        void OnUpdateRuntime(TimeStep ts);
        void OnUpdateSimulation(TimeStep ts, EditorCamera& camera);

        void SetPaused(bool paused) { m_IsPaused = paused; }
        bool IsPaused() const { return m_IsPaused; }

        void Step(int frames = 1);
    private:
        void RenderSceneContext();
        void RenderScene(EditorCamera& camera);

        template<typename T>
        void OnComponentAdded(Entity entity, T& component);

        void OnPhysics2DStart();
        void OnPhysics2DStop();
        void PhysicsScene(TimeStep ts);
    private:
        uint32_t m_ViewportWidth = 0, m_ViewportHeight = 0;

        entt::registry m_Registry;
        std::unordered_map<UUID, entt::entity> m_EntityMap;
        friend class Entity;
        friend class SceneSerializer;
        friend class SceneHierarchyPanel;

        bool m_IsPaused = false;
        int m_StepFrames = 0;

        b2World* m_PhysicsWorld = nullptr;
    };
}
