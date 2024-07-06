#include "sdpch.h"
#include "Shadow/Renderer/FrameBuffer.h"
#include "Shadow/Renderer/Renderer.h"

#include "Platform/OpenGL/OpenGLFrameBuffer.h"

namespace Shadow
{
    Ref<FrameBuffer> FrameBuffer::Create(const FrameBufferSpecification& spec)
    {
        switch (Renderer::GetAPI())
        {
            case RendererAPI::API::None:    SD_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
            case RendererAPI::API::OpenGL:  return CreateRef<OpenGLFrameBuffer>(spec);
        }

        SD_CORE_ASSERT(false, "Unknown RendererAPI!");
        return nullptr;
    }

}
