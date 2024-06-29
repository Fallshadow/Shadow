#include "sdpch.h"
#include "Shadow/Renderer/RenderCommand.h"

namespace Shadow
{
    Scope<RendererAPI> RenderCommand::s_RendererAPI = RendererAPI::Create();
}