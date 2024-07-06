#pragma once

#include "Shadow/Renderer/FrameBuffer.h"

namespace Shadow
{
    class OpenGLFrameBuffer : public FrameBuffer
    {
    public:
        OpenGLFrameBuffer(const FrameBufferSpecification& spec);
        virtual ~OpenGLFrameBuffer();

        virtual void Bind() override;
        virtual void Unbind() override;

        // 重置窗口大小
        virtual void Resize(uint32_t width, uint32_t height) override;
        // 读取附加颜色缓冲区某坐标像素数据（用来获取点击的实体）
        virtual int ReadPixel(uint32_t attachmentIndex, int x, int y) override;

        // 以某数据重置纹理图像
        virtual void ClearAttachment(uint32_t attachmentIndex, int value) override;

        virtual uint32_t GetColorAttachmentRendererID(uint32_t index = 0) const override
        {
            SD_CORE_ASSERT(index < m_ColorAttachments.size());
            return m_ColorAttachments[index];
        }

        virtual const FrameBufferSpecification& GetSpecification() const override { return m_Specification; }

    private:
        void InvalidCheckAndBindFrameBuffer();
        void DeleteFrameBufferAndTextures();
    private:
        uint32_t m_RendererID = 0;
        // 当前帧缓冲数据（纹理数据集）
        FrameBufferSpecification m_Specification;

        // 颜色纹理对象名称的存储（感觉像是指针）
        std::vector<uint32_t> m_ColorAttachments;
        // 深度纹理对象名称的存储（感觉像是指针）
        uint32_t m_DepthAttachment = 0;

        // 颜色枚举
        std::vector<FrameBufferTextureSpecification> m_ColorAttachmentSpecifications;
        // 深度枚举
        FrameBufferTextureSpecification m_DepthAttachmentSpecification = FrameBufferTextureFormat::None;
    };
}
