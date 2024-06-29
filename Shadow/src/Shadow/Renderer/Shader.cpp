#include "sdpch.h"
#include "Shadow/Renderer/Renderer.h"
#include "Shadow/Renderer/Shader.h"
#include "Platform/OpenGL/OpenGLShader.h"


namespace Shadow
{
    Ref<Shader> Shader::Create(const std::string& fliepath)
    {
        switch (Renderer::GetAPI())
        {
        case RendererAPI::API::None:	SD_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
        case RendererAPI::API::OpenGL:	return CreateRef<OpenGLShader>(fliepath);
        }

        SD_CORE_ASSERT(false, "Unknown RendererAPI!");
        return nullptr;
    }

    Ref<Shader> Shader::Create(const std::string& name, const std::string& vertexStr, const std::string& fragmentStr)
    {
        switch (Renderer::GetAPI())
        {
        case RendererAPI::API::None:	SD_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
        case RendererAPI::API::OpenGL:	return CreateRef<OpenGLShader>(name, vertexStr, fragmentStr);
        }

        SD_CORE_ASSERT(false, "Unknown RendererAPI!");
        return nullptr;
    }

    Ref<Shader> ShaderLibrary::Load(const std::string& filepath)
    {
        auto shader = Shader::Create(filepath);
        Add(shader);
        return shader;
    }

    Ref<Shader> ShaderLibrary::Load(const std::string& name, const std::string& filepath)
    {
        auto shader = Shader::Create(filepath);
        Add(name, shader);
        return shader;
    }

    void ShaderLibrary::Add(const Ref<Shader>& shader)
    {
        auto& name = shader->GetName();
        m_Shaders[name] = shader;
    }

    void ShaderLibrary::Add(const std::string& name, const Ref<Shader>& shader)
    {
        SD_CORE_ASSERT(!Exists(name), "Shader already exists!");
        m_Shaders[name] = shader;
    }

    Ref<Shader> ShaderLibrary::Get(const std::string& name)
    {
        SD_CORE_ASSERT(Exists(name), "Shader not found!");
        return m_Shaders[name];
    }

    bool ShaderLibrary::Exists(const std::string& name) const
    {
        return m_Shaders.find(name) != m_Shaders.end();
    }
}