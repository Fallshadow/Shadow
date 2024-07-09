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
    };
}
