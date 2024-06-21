#include "sdpch.h"
#include "Shadow/Core/Application.h"

namespace Shadow
{
	Application* Application::s_Instance = nullptr;


	Application::Application()
	{
		s_Instance = this;

        // 窗口创建和事件注册
        m_Window = Window::Create(WindowProps());
        m_Window->SetEventCallback(SD_BIND_EVENT_FN(Application::OnEvent));

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

			// 主线程队列代码执行
            ExecuteMainThreadQueue();

			// TODO:各个功能层执行

			// 窗口更新
            m_Window->OnUpdate();
		}
	}

	void Application::Close()
	{
		m_Running = false;
	}

    void Application::OnEvent(Event& e)
    {
        EventDispatcher dispatcher(e);
        dispatcher.Dispatch<WindowCloseEvent>(SD_BIND_EVENT_FN(Application::OnWindowClose));
        dispatcher.Dispatch<WindowResizeEvent>(SD_BIND_EVENT_FN(Application::OnWindowResize));
    }

    bool Application::OnWindowClose(WindowCloseEvent& e)
    {
        m_Running = false;
        return true;
    }

    bool Application::OnWindowResize(WindowResizeEvent& e)
    {
        // 除了普通缩放 0,0 窗口最小化也是 0,0
        if (e.GetWidth() == 0 || e.GetHeight() == 0)
        {
            m_Minimized = true;
            return false;
        }

        m_Minimized = false;
        return false;
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