#pragma once

#include "Shadow/Core/Layer.h"

#include "Shadow/Events/ApplicationEvent.h"
#include "Shadow/Events/KeyEvent.h"
#include "Shadow/Events/MouseEvent.h"

namespace Shadow
{
    class ImGuiLayer : public Layer
    {
    public:
        ImGuiLayer();
        ~ImGuiLayer() = default;

        virtual void OnAttach() override;
        virtual void OnDetach() override;
        virtual void OnEvent(Event& e) override;

        void Begin();
        void End();

        void BlockEvents(bool block) { m_BlockEvents = block; }

        uint32_t GetActiveWidgetID() const;
    private:
        bool m_BlockEvents = true;
    };
}
