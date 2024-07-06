#include "sdpch.h"
#include "Shadow/Renderer/UniformBuffer.h"
#include "Shadow/Renderer/Renderer.h"
#include "Platform/OpenGL/OpenGLUniformBuffer.h"

namespace Shadow
{

    Ref<UniformBuffer> UniformBuffer::Create(uint32_t size, uint32_t binding)
    {
        switch (Renderer::GetAPI())
        {
        case RendererAPI::API::None:    SD_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
        case RendererAPI::API::OpenGL:  return CreateRef<OpenGLUniformBuffer>(size, binding);
        }

        SD_CORE_ASSERT(false, "Unknown RendererAPI!");
        return nullptr;
    }

}
