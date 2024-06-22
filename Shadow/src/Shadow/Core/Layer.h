#pragma once

#include "Shadow/Core/Base.h"
#include "Shadow/Events/Event.h"
#include "Shadow/Core/Tool/Timestep.h"

#include <string>

namespace Shadow
{
    class Layer
    {
    public:
        Layer(const std::string& name = "Layer");
        virtual ~Layer() = default;

        virtual void OnAttach() {}
        virtual void OnDetach() {}
        virtual void OnUpdate(TimeStep ts) {}

        virtual void OnEvent(Event& event) {}
        virtual void OnImGuiRender() {}
        

        const std::string& GetName() const { return m_Name; }
    private:
        std::string m_Name;
    };
}