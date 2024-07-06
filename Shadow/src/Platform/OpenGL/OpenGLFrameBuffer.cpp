#include "sdpch.h"
#include "Platform/OpenGL/OpenGLFrameBuffer.h"

#include <glad/glad.h>

namespace Shadow
{
    static const uint32_t s_MaxFrameBufferSize = 8192;

    namespace Utils
    {
        static GLenum TextureTarget(bool multisampled)
        {
            return multisampled ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;
        }

        /// <summary>
        /// 创建贴图
        /// </summary>
        /// <param name="multisampled">是否多重采样</param>
        /// <param name="count">贴图数量</param>
        /// <param name="outID">贴图数据</param>
        static void CreateTextures(bool multisampled, uint32_t count, uint32_t* outID)
        {
            glCreateTextures(TextureTarget(multisampled), count, outID);
        }

        /// <summary>
        /// 绑定纹理
        /// </summary>
        /// <param name="multisampled">是否多重采样</param>
        /// <param name="id">指定纹理名称</param>
        static void BindTexture(bool multisampled, uint32_t id)
        {
            glBindTexture(TextureTarget(multisampled), id);
        }

        /// <summary>
        /// 将颜色纹理绑定到帧缓冲
        /// </summary>
        static void AttachColorTexture(uint32_t id, int samples, GLenum internalFormat, GLenum format, uint32_t width, uint32_t height, int index)
        {
            bool multisampled = samples > 1;
            if (multisampled)
            {
                // 建立多样本纹理图像的数据存储、格式、尺寸和样本数量
                // 最后一个参数指定图像是否对图像中的所有纹素使用相同的采样位置和相同数量的采样，并且采样位置不依赖于图像的内部格式或大小。
                glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, internalFormat, width, height, GL_FALSE);

                // 多重采样纹理是一种用于进行抗锯齿（anti-aliasing）的技术，它在采样过程中会对纹理进行多次采样，从而减少锯齿现象的出现。
                // 由于多重采样纹理本身已经包含了多次采样的信息，所以通常不需要设置额外的纹理过滤（texture filtering）和环绕模式（wrap mode）。
            }
            else
            {
                // 指定二维纹理图像
                // 1：指定目标纹理
                // 2：指定详细级别编号。级别 0 是基本映像级别。级别 n 是第 n 个 mipmap 缩减图像。
                // 3，4，5：内部格式，宽高
                // 6：必须为0
                // 7，8：指定像素数据的格式、数据类型
                // 9：指定指向内存中图像数据的指针。
                glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, GL_UNSIGNED_BYTE, nullptr);

                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            }

            // glFramebufferTexture2D 函数是OpenGL用于将一个纹理对象附加到帧缓冲对象（framebuffer object）上的函数。
            // 它的作用是将一个特定类型的纹理对象附加到当前绑定的帧缓冲对象的特定附着点（attachment point）上。
            // 1：target：指定帧缓冲的目标，通常为 GL_FRAMEBUFFER。
            // 2：attachment：指定要附加纹理的附着点（attachment point），可以是以下值之一：
            // GL_COLOR_ATTACHMENT0: 将纹理附加到帧缓冲的颜色附着点0上。这里用index将所有纹理都依次附着到帧缓冲中。
            // GL_DEPTH_ATTACHMENT : 将纹理附加到帧缓冲的深度附着点上。
            // GL_STENCIL_ATTACHMENT : 将纹理附加到帧缓冲的模板附着点上。
            // GL_DEPTH_STENCIL_ATTACHMENT : 将纹理附加到帧缓冲的深度和模板附着点上（深度模板一体）。
            // 3：texTarget：指定纹理的类型，一般为 GL_TEXTURE_2D。
            // 4：texture：指定要附加到帧缓冲的纹理对象。
            // 5：level：指定要附加的纹理的mipmap级别（通常为0，表示基本级别）。
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + index, TextureTarget(multisampled), id, 0);
        }

        /// <summary>
        /// 将深度纹理绑定到缓冲帧
        /// </summary>
        static void AttachDepthTexture(uint32_t id, int samples, GLenum format, uint32_t width, uint32_t height, GLenum attachmentType)
        {
            bool multisampled = samples > 1;
            if (multisampled)
            {
                glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, format, width, height, GL_FALSE);
            }
            else
            {
                // 用来分配并初始化纹理对象内存空间的函数。它可以为指定的纹理对象创建一个二维纹理资源，并为其分配一定的内存空间。
                // 2：levels：指定纹理的多级渐远纹理级别数量（mipmaps levels）。通常设置为1，表示只有一个基本级别。
                glTexStorage2D(GL_TEXTURE_2D, 1, format, width, height);

                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            }

            glFramebufferTexture2D(GL_FRAMEBUFFER, attachmentType, TextureTarget(multisampled), id, 0);
        }

        static bool IsDepthFormat(FrameBufferTextureFormat format)
        {
            switch (format)
            {
                case FrameBufferTextureFormat::DEPTH24STENCIL8:  return true;
            }

            return false;
        }

        static GLenum HazelFBTextureFormatToGL(FrameBufferTextureFormat format)
        {
            switch (format)
            {
            case FrameBufferTextureFormat::RGBA8:       return GL_RGBA8;
            case FrameBufferTextureFormat::RED_INTEGER: return GL_RED_INTEGER;
            }

            SD_CORE_ASSERT(false);
            return 0;
        }
    }

    OpenGLFrameBuffer::OpenGLFrameBuffer(const FrameBufferSpecification& spec) : m_Specification(spec)
    {
        for (auto spec : m_Specification.Attachments.Attachments)
        {
            if (!Utils::IsDepthFormat(spec.TextureFormat))
                m_ColorAttachmentSpecifications.emplace_back(spec);
            else
                m_DepthAttachmentSpecification = spec;
        }

        InvalidCheckAndBindFrameBuffer();
    }

    OpenGLFrameBuffer::~OpenGLFrameBuffer()
    {
        DeleteFrameBufferAndTextures();
    }

    void OpenGLFrameBuffer::Bind()
    {
        glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID);
        glViewport(0, 0, m_Specification.Width, m_Specification.Height);
    }

    void OpenGLFrameBuffer::Unbind()
    {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    // 重置窗口大小需要更新帧缓冲宽高
    void OpenGLFrameBuffer::Resize(uint32_t width, uint32_t height)
    {
        if (width == 0 || height == 0 || width > s_MaxFrameBufferSize || height > s_MaxFrameBufferSize)
        {
            SD_CORE_WARN("Attempted to rezize framebuffer to {0}, {1}", width, height);
            return;
        }

        m_Specification.Width = width;
        m_Specification.Height = height;

        InvalidCheckAndBindFrameBuffer();
    }

    int OpenGLFrameBuffer::ReadPixel(uint32_t attachmentIndex, int x, int y)
    {
        SD_CORE_ASSERT(attachmentIndex < m_ColorAttachments.size());

        // 用于指定读取像素数据时使用的颜色缓冲区。
        // 它可以用来指定当前上下文中读取像素数据时使用的颜色缓冲区
        glReadBuffer(GL_COLOR_ATTACHMENT0 + attachmentIndex);
        int pixelData;
        // 将帧缓冲区中的像素数据读取到内存中。
        // 它可以用来获取渲染后的像素数据，以便进一步处理或保存。
        glReadPixels(x, y, 1, 1, GL_RED_INTEGER, GL_INT, &pixelData);
        return pixelData;
    }

    void OpenGLFrameBuffer::ClearAttachment(uint32_t attachmentIndex, int value)
    {
        SD_CORE_ASSERT(attachmentIndex < m_ColorAttachments.size());

        auto& spec = m_ColorAttachmentSpecifications[attachmentIndex];
        // 用于清除纹理的图像数据。它可以将指定的纹理对象的图像数据设置为特定的值。
        // 需要注意的是，glClearTexImage 函数仅能用于清除纹理对象中的图像数据，而不会影响纹理对象的其他属性（如参数设置、绑定状态等）。
        // 在使用 glClearTexImage 函数前，需要确保纹理对象已经创建和绑定，同时要根据实际需求设置合适的参数和数据类型，以确保清除操作能够正确进行。
        // 总的来说，glClearTexImage 函数是用于清除纹理图像数据的函数，可用于重置纹理数据或初始化纹理数据等操作。
        // texture：要清除图像数据的纹理对象。
        // level：要清除的纹理级别。
        // format：指定数据的像素格式。
        // type：指定数据的数据类型。
        // data：指定要用来清除图像数据的数值。
        glClearTexImage(m_ColorAttachments[attachmentIndex], 0, Utils::HazelFBTextureFormatToGL(spec.TextureFormat), GL_INT, &value);
    }


    // 判断渲染存在与否 --
    // 是 -> 删除已存在的渲染 -> 
    // 创建帧缓冲，绑定帧缓冲 -> 
    // 使用贴图数据为帧缓冲附加 颜色和深度 信息 ->
    // 判断是否存在颜色附加点信息 --
    // 是 -> 使用设置好的帧缓冲颜色附加点绘制颜色缓冲区
    // 否 -> 仅仅绘制深度
    // 检查帧缓冲完整性 ->
    // 解绑帧缓冲，将信息回显到屏幕上
    void OpenGLFrameBuffer::InvalidCheckAndBindFrameBuffer()
    {
        // 如果已经存在一个渲染了，就删除它
        if (m_RendererID)
        {
            DeleteFrameBufferAndTextures();
        }

        glCreateFramebuffers(1, &m_RendererID);
        glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID);

        bool multisample = m_Specification.Samples > 1;

        if (m_ColorAttachmentSpecifications.size())
        {
            // 根据颜色枚举创建贴图数据
            m_ColorAttachments.resize(m_ColorAttachmentSpecifications.size());
            Utils::CreateTextures(multisample, m_ColorAttachments.size(), m_ColorAttachments.data());

            for (size_t i = 0; i < m_ColorAttachments.size(); i++)
            {
                Utils::BindTexture(multisample, m_ColorAttachments[i]);
                switch (m_ColorAttachmentSpecifications[i].TextureFormat)
                {
                case FrameBufferTextureFormat::RGBA8:
                    // RGBA8 一个像素格式，表示每个像素包含红、绿、蓝和透明度（alpha）四个分量。
                    // RGBA8 使用 8 位无符号整数来表示每个分量，范围从 0 到 255。
                    Utils::AttachColorTexture(m_ColorAttachments[i], m_Specification.Samples, GL_RGBA8, GL_RGBA, m_Specification.Width, m_Specification.Height, i);
                    break;
                case FrameBufferTextureFormat::RED_INTEGER:
                    // GL_R32I 表示一个单通道的整数纹理，其中每个像素的红色分量使用 32 位有符号整数来表示
                    Utils::AttachColorTexture(m_ColorAttachments[i], m_Specification.Samples, GL_R32I, GL_RED_INTEGER, m_Specification.Width, m_Specification.Height, i);
                    break;
                }
            }
        }

        if (m_DepthAttachmentSpecification.TextureFormat != FrameBufferTextureFormat::None)
        {
            // 根据深度枚举创建贴图数据
            Utils::CreateTextures(multisample, 1, &m_DepthAttachment);
            Utils::BindTexture(multisample, m_DepthAttachment);
            switch (m_DepthAttachmentSpecification.TextureFormat)
            {
            case FrameBufferTextureFormat::DEPTH24STENCIL8:
                Utils::AttachDepthTexture(m_DepthAttachment, m_Specification.Samples, GL_DEPTH24_STENCIL8, m_Specification.Width, m_Specification.Height, GL_DEPTH_STENCIL_ATTACHMENT);
                break;
            }
        }

        // 通过调用 glDrawBuffers 函数，可以指定帧缓冲对象中的多个颜色附着点应绘制到哪些颜色缓冲区上。
        // 这样，OpenGL在绘制过程中会将对应的颜色输出到指定的颜色缓冲区中，从而实现多个颜色附着点的绘制目标设置。
        if (m_ColorAttachments.size() > 1)
        {
            SD_CORE_ASSERT(m_ColorAttachments.size() <= 4);
            GLenum buffers[4] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 };
            glDrawBuffers(m_ColorAttachments.size(), buffers);
        }
        else if (m_ColorAttachments.empty())
        {
            // 仅仅绘制深度 听说废弃了，看看有没有代替的方法
            glDrawBuffer(GL_NONE);
        }

        // glCheckFramebufferStatus(GL_FRAMEBUFFER) 是用于检查帧缓冲对象的完整性的函数。
        // 该函数会返回当前绑定的帧缓冲对象的完整性检查结果，以确定帧缓冲对象是否已经正确配置并可以使用。
        SD_CORE_ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE, "FrameBuffer is incomplete!");

        // glBindFramebuffer是一个OpenGL函数，用于将一个帧缓冲对象绑定到上下文的绑定点。
        // 帧缓冲对象是一个用来渲染或捕捉图像数据的对象，它包含颜色缓冲区、深度缓冲区、模板缓冲区等。
        // 在渲染管线中，可以使用帧缓冲对象来进行离屏渲染、多重采样渲染、延迟着色等操作。

        // 其中 GL_FRAMEBUFFER 是绑定目标，0 是帧缓冲对象的标识符，表示绑定默认的帧缓冲对象，即将渲染目标切换回窗口系统提供的默认帧缓冲区（通常是前/后缓冲区）。
        // 通过将帧缓冲对象绑定到 0，即可将渲染操作重定向到屏幕，使得后续的渲染操作将直接呈现在屏幕上，而不再是之前绑定的离屏渲染目标。
        // 这通常在完成离屏渲染或渲染到纹理后，需要将渲染目标切换回屏幕显示时使用。
        // 总的来说，这行代码可用于解绑定之前绑定的帧缓冲对象，将渲染目标恢复到默认的帧缓冲区，以便后续的渲染内容能够直接显示在屏幕上。
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void OpenGLFrameBuffer::DeleteFrameBufferAndTextures()
    {
        glDeleteFramebuffers(1, &m_RendererID);
        glDeleteTextures(m_ColorAttachments.size(), m_ColorAttachments.data());
        glDeleteTextures(1, &m_DepthAttachment);

        m_ColorAttachments.clear();
        m_DepthAttachment = 0;
    }
}
