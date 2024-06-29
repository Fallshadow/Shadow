#pragma once
#include <mutex>
#include <vector>
#include <functional>

#include "Shadow/Core/Window.h"
#include "Shadow/Core/Layer.h"
#include "Shadow/Core/LayerStack.h"
#include "Shadow/Events/Event.h"
#include "Shadow/Events/ApplicationEvent.h"
#include "Shadow/ImGui/ImGuiLayer.h"

int main(int argc, char** argv);

namespace Shadow 
{
	class Application
	{
	public:
		Application();
		~Application();

		void Close();

        static Application& Get() { return *s_Instance; }
        ImGuiLayer* GetImGuiLayer() { return m_ImGuiLayer; }
        Window& GetWindow() { return *m_Window; }

        void PushLayer(Layer* layer);
        void PushOverlay(Layer* overlay);

        void OnEvent(Event& e);

		void SubmitToMainThread(const std::function<void()>& function);
	private:
		void Run();
		
		void ExecuteMainThreadQueue();
	private:
		bool m_Running = true;

		std::vector<std::function<void()>> m_MainThreadQueue;
		std::mutex m_MainThreadQueueMutex;

        LayerStack m_LayerStack;

        float m_LastFrameTime = 0.0f;

        Scope<Window> m_Window;
        ImGuiLayer* m_ImGuiLayer;
        bool OnWindowClose(WindowCloseEvent& e);
        bool OnWindowResize(WindowResizeEvent& e);
        bool m_Minimized = false;

		static Application* s_Instance;
		friend int ::main(int argc, char** argv);
	};

	Application* CreateApplication();
}