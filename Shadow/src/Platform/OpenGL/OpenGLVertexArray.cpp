#include "sdpch.h"
#include "Platform/OpenGL/OpenGLVertexArray.h"
#include "glad/glad.h"

namespace Shadow
{
    static GLenum ShaderDataTypeToOpenGLBaseType(ShaderDataType type)
    {
        switch (type)
        {
            case ShaderDataType::Float:		return GL_FLOAT;
            case ShaderDataType::Float2:	return GL_FLOAT;
            case ShaderDataType::Float3:	return GL_FLOAT;
            case ShaderDataType::Float4:	return GL_FLOAT;
            case ShaderDataType::Mat3:		return GL_FLOAT;
            case ShaderDataType::Mat4:		return GL_FLOAT;
            case ShaderDataType::Int:		return GL_INT;
            case ShaderDataType::Int2:		return GL_INT;
            case ShaderDataType::Int3:		return GL_INT;
            case ShaderDataType::Int4:		return GL_INT;
            case ShaderDataType::Bool:		return GL_BOOL;
        }

        SD_CORE_ASSERT(false, "Unknown ShaderDataType!");
        return 0;
    }

    OpenGLVertexArray::OpenGLVertexArray()
    {
        glCreateVertexArrays(1, &m_RendererID);
    }

    OpenGLVertexArray::~OpenGLVertexArray()
    {
        glDeleteVertexArrays(1, &m_RendererID);
    }

    void OpenGLVertexArray::Bind() const
    {
        glBindVertexArray(m_RendererID);
    }

    void OpenGLVertexArray::UnBind() const
    {
        glBindVertexArray(0);
    }

    void OpenGLVertexArray::AddVertexBuffer(const Ref<VertexBuffer>& vb)
    {
        SD_CORE_ASSERT(vb->GetLayout().GetElements().size(), "Vertex Buffer has no layout!");

        glBindVertexArray(m_RendererID);
        vb->Bind();

        const auto& layout = vb->GetLayout();
        for (const auto& e : layout)
        {
            switch (e.Type)
            {
            case ShaderDataType::Float:
            case ShaderDataType::Float2:
            case ShaderDataType::Float3:
            case ShaderDataType::Float4:
            {
                glEnableVertexAttribArray(m_VertexBufferIndex);
                glVertexAttribPointer(m_VertexBufferIndex,
                    e.GetComponentCount(),
                    ShaderDataTypeToOpenGLBaseType(e.Type),
                    e.Normalized ? GL_TRUE : GL_FALSE,
                    layout.GetStride(),
                    (const void*)e.Offset);
                m_VertexBufferIndex++;
                break;
            }
            case ShaderDataType::Int:
            case ShaderDataType::Int2:
            case ShaderDataType::Int3:
            case ShaderDataType::Int4:
            case ShaderDataType::Bool:
            {
                glEnableVertexAttribArray(m_VertexBufferIndex);
                glVertexAttribIPointer(m_VertexBufferIndex,
                    e.GetComponentCount(),
                    ShaderDataTypeToOpenGLBaseType(e.Type),
                    layout.GetStride(),
                    (const void*)e.Offset);
                m_VertexBufferIndex++;
                break;
            }
            case ShaderDataType::Mat3:
            case ShaderDataType::Mat4:
            {
                uint8_t count = e.GetComponentCount();
                for (uint8_t i = 0; i < count; i++)
                {
                    glEnableVertexAttribArray(m_VertexBufferIndex);
                    glVertexAttribPointer(m_VertexBufferIndex,
                        count,
                        ShaderDataTypeToOpenGLBaseType(e.Type),
                        e.Normalized ? GL_TRUE : GL_FALSE,
                        layout.GetStride(),
                        (const void*)(e.Offset + sizeof(float) * count * i));
                    glVertexAttribDivisor(m_VertexBufferIndex, 1);
                    m_VertexBufferIndex++;
                }
                break;
            }
            default:
                SD_CORE_ASSERT(false, "Unknown ShaderDataType!");
            }
        }
        m_VertexBuffers.push_back(vb);
    }

    void OpenGLVertexArray::SetIndexBuffer(const Ref<IndexBuffer>& ib)
    {
        glBindVertexArray(m_RendererID);
        ib->Bind();

        m_IndexBuffer = ib;
    }
}
