#pragma once

#include "Shadow/Renderer/EditorCamera.h"
#include "Shadow/Core/UUID.h"
#include "Shadow/Scene/Entity.h"

#include <entt.hpp>

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
        void RenderScene(EditorCamera& camera);

        template<typename T>
        void OnComponentAdded(Entity entity, T& component);
    private:
        uint32_t m_ViewportWidth = 0, m_ViewportHeight = 0;

        entt::registry m_Registry;
        std::unordered_map<UUID, entt::entity> m_EntityMap;
        friend class Entity;
        friend class SceneSerializer;
        friend class SceneHierarchyPanel;

        bool m_IsPaused = false;
        int m_StepFrames = 0;
    };
}
