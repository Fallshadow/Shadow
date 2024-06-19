#pragma once
#include <mutex>
#include <vector>
#include <functional>

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

		void SubmitToMainThread(const std::function<void()>& function);
	private:
		void Run();
		
		void ExecuteMainThreadQueue();
	private:
		bool m_Running = true;

		std::vector<std::function<void()>> m_MainThreadQueue;
		std::mutex m_MainThreadQueueMutex;

		static Application* s_Instance;
		friend int ::main(int argc, char** argv);
	};

	Application* CreateApplication();
}