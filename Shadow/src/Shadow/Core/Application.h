#pragma once
#include <mutex>
#include <vector>
#include <functional>

#include "Shadow/Core/Window.h"
#include "Shadow/Events/Event.h"
#include "Shadow/Events/ApplicationEvent.h"

int main(int argc, char** argv);

namespace Shadow 
{
	class Application
	{
	public:
		Application();
		~Application();

		void Close();

        Window& GetWindow() { return *m_Window; }
        void OnEvent(Event& e);

		static Application& Get() { return *s_Instance; }

		void SubmitToMainThread(const std::function<void()>& function);
	private:
		void Run();
		
		void ExecuteMainThreadQueue();
	private:
		bool m_Running = true;

		std::vector<std::function<void()>> m_MainThreadQueue;
		std::mutex m_MainThreadQueueMutex;

        Scope<Window> m_Window;
        bool OnWindowClose(WindowCloseEvent& e);
        bool OnWindowResize(WindowResizeEvent& e);
        bool m_Minimized = false;

		static Application* s_Instance;
		friend int ::main(int argc, char** argv);
	};

	Application* CreateApplication();
}