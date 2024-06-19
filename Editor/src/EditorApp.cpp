#include <Shadow.h>
#include <Shadow/Core/EntryPoint.h>

namespace Shadow
{
	class Editor : public Application
	{
	public:
		Editor() {}
	};

	Application* CreateApplication()
	{
		return new Editor();
	}
}