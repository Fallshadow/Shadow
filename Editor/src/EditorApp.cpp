#include <Shadow.h>
#include <Shadow/Core/EntryPoint.h>

#include "EditorLayer.h"
#include "UnitTest/UT1FocusRendererLayer.h"

namespace Shadow
{
	class EditorApp : public Application
	{
	public:
        EditorApp()
        {
            PushLayer(new EditorLayer());
            PushLayer(new UT1FocusRendererLayer());
        }
	};

	Application* CreateApplication()
	{
		return new EditorApp();
	}
}