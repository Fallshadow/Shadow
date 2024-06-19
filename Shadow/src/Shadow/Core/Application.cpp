#include "sdpch.h"
#include "Shadow/Core/Application.h"


namespace Shadow
{
	Application* Application::s_Instance = nullptr;


	Application::Application()
	{
		s_Instance = this;

		// 创建窗口
		// 窗口事件注册
		// 渲染初始化
		// Imgui初始化
	}

	Application::~Application()
	{
		// 脚本结束
		// 渲染结束
	}

	void Application::Run()
	{
		while (m_Running)
		{
			// TODO:Time步进

			// TODO:主线程队列代码执行

			// TODO:各个功能层执行

			// TODO:窗口更新
		}
	}

	void Application::Close()
	{
		m_Running = false;
	}

	void Application::SubmitToMainThread(const std::function<void()>& function)
	{
		std::scoped_lock<std::mutex> lock(m_MainThreadQueueMutex);
	}

	void Application::ExecuteMainThreadQueue()
	{
		std::scoped_lock<std::mutex> lock(m_MainThreadQueueMutex);

		for (auto& func : m_MainThreadQueue)
			func();

		m_MainThreadQueue.clear();
	}
}