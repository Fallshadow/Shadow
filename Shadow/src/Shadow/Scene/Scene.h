#pragma once

#include "Shadow/Renderer/EditorCamera.h"

namespace Shadow
{
    class Scene
    {
    public:
        Scene();
        ~Scene();
        static Ref<Scene> Copy(Ref<Scene> other);

        void OnUpdateEditor(EditorCamera& camera);
        void OnViewportResize(uint32_t width, uint32_t height);
    private:
        void RenderScene(EditorCamera& camera);
    private:
        uint32_t m_ViewportWidth = 0, m_ViewportHeight = 0;
    };
}
