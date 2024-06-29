#include "sdpch.h"
#include "Shadow/Renderer/Texture.h"
#include "Shadow/Renderer/Renderer.h"
#include "Platform/OpenGL/OpenGLTexture.h"

namespace Shadow
{
    Ref<Texture2D> Texture2D::Create(const TextureSpecification& specification)
    {
        switch (Renderer::GetAPI())
        {
            case RendererAPI::API::None:    SD_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
            case RendererAPI::API::OpenGL:  return CreateRef<OpenGLTexture2D>(specification);
        }

        SD_CORE_ASSERT(false, "Unknown RendererAPI!");
        return nullptr;
    }

    Ref<Texture2D> Texture2D::Create(const std::string& path)
    {
        switch (Renderer::GetAPI())
        {
            case RendererAPI::API::None:	SD_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
            case RendererAPI::API::OpenGL:	return std::make_shared<OpenGLTexture2D>(path);
        }
        return Ref<Texture2D>();
    }
}
