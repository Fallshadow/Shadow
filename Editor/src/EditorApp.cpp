#include <Shadow.h>
#include <Shadow/Core/EntryPoint.h>

#include "EditorLayer.h"

namespace Shadow
{
	class EditorApp : public Application
	{
	public:
        EditorApp()
        {
            PushLayer(new EditorLayer());
        }
	};

	Application* CreateApplication()
	{
		return new EditorApp();
	}
}