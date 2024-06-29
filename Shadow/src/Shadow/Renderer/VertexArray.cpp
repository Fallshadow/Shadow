#include "sdpch.h"
#include "Shadow/Renderer/VertexArray.h"
#include "Shadow/Renderer/Renderer.h"
#include "Platform/OpenGL/OpenGLVertexArray.h"

namespace Shadow
{
    Ref<VertexArray> VertexArray::Create()
    {
        switch (RendererAPI::GetAPI())
        {
            case RendererAPI::API::None:	SD_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
            case RendererAPI::API::OpenGL:	return CreateRef<OpenGLVertexArray>();
        }

        SD_CORE_ASSERT(false, "Unknown RendererAPI");
        return nullptr;
    }
}