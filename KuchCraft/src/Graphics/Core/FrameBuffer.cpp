#include "kcpch.h"
#include "Graphics/Core/FrameBuffer.h"

#include "Graphics/Core/GraphicsUtils.h"

#include <glad/glad.h>

namespace KuchCraft {

	KC_TODO("Add asserts and logs to frame buffer");

	FrameBuffer::~FrameBuffer()
	{
		DeleteTextures();
	}

	FrameBuffer::FrameBuffer(const FrameBufferSpecification& spec)
		: m_Specification(spec)
	{
		Invalidate();
		SetDebugNames();
	}

	Ref<FrameBuffer> FrameBuffer::Create(const FrameBufferSpecification& spec)
	{
		return Ref<FrameBuffer>(new FrameBuffer(spec));
	}

	void FrameBuffer::Bind() const
	{
		glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID);
		glViewport(0, 0, m_Specification.Width, m_Specification.Height);
	}

	void FrameBuffer::Unbind() const
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void FrameBuffer::BindColorTexture(int slot, int attachmentIndex) const
	{
		glBindTextureUnit(slot, m_ColorAttachmentsRendererID[attachmentIndex]);
	}

	void FrameBuffer::BindDepthTexture(int slot) const
	{
		glBindTextureUnit(slot, m_DepthAttachmentRendererID);
	}

	void FrameBuffer::BindTexture(int slot, const std::string& name) const
	{
		glBindTextureUnit(slot, GetAttachmentRendererID(name));
	}

	void FrameBuffer::Resize(int width, int height)
	{
		if (m_Specification.NoResize)
			return;

		if (width == 0 || height == 0 || (m_Specification.Width == width && m_Specification.Height == height))
			return;
		 
		m_Specification.Width  = width;
		m_Specification.Height = height;

		Invalidate();
	}

	static GLbitfield ConvertBlitMaskToGL(FrameBufferBlitMask mask)
	{
		GLbitfield result = 0;

		if ((static_cast<uint32_t>(mask) & static_cast<uint32_t>(FrameBufferBlitMask::Color)) != 0)
			result |= GL_COLOR_BUFFER_BIT;

		if ((static_cast<uint32_t>(mask) & static_cast<uint32_t>(FrameBufferBlitMask::Depth)) != 0)
			result |= GL_DEPTH_BUFFER_BIT;

		if ((static_cast<uint32_t>(mask) & static_cast<uint32_t>(FrameBufferBlitMask::Stencil)) != 0)
			result |= GL_STENCIL_BUFFER_BIT;

		return result;
	}

	void FrameBuffer::BlitTo(Ref<FrameBuffer> target, FrameBufferBlitMask mask, TextureFilter filter) const
	{
		glBindFramebuffer(GL_READ_FRAMEBUFFER, m_RendererID);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, target->GetRendererID());

		glBlitFramebuffer(
			0, 0, m_Specification.Width, m_Specification.Height,
			0, 0, target->GetWidth(), target->GetHeight(),
			ConvertBlitMaskToGL(mask), Utils::GetGlTextureFilter(filter)
		);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void FrameBuffer::BlitToDefault(FrameBufferBlitMask mask, TextureFilter filter) const
	{
		glBindFramebuffer(GL_READ_FRAMEBUFFER, m_RendererID);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

		glBlitFramebuffer(
			0, 0, m_Specification.Width, m_Specification.Height,
			0, 0, m_Specification.Width, m_Specification.Height,
			ConvertBlitMaskToGL(mask), Utils::GetGlTextureFilter(filter)
		);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void FrameBuffer::SetDrawBuffer(GLenum buffer)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID);
		glDrawBuffer(buffer);
	}

	void FrameBuffer::SetDrawBuffers(std::initializer_list<GLenum> buffers)
	{
		std::vector<GLenum> list(buffers);
		glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID);
		glDrawBuffers((GLsizei)list.size(), list.data());
	}

	void FrameBuffer::ReadPixels(int attachmentIndex, int x, int y, int width, int height, void* outData) const
	{
		if (!HasColorAttachment(attachmentIndex))
		{
			KC_CORE_WARN("Invalid color attachment index {}", attachmentIndex);
			return;
		}

		glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID);
		glReadBuffer(GL_COLOR_ATTACHMENT0 + attachmentIndex);

		auto& spec = m_ColorAttachmentSpecifications[attachmentIndex];
		GLenum format = Utils::GetGLFormat(spec.Format);
		GLenum type = Utils::GetGLType(spec.Format);

		glReadPixels(x, y, width, height, format, type, outData);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void FrameBuffer::ReadDepthPixels(int x, int y, int width, int height, float* outData) const
	{
		if (m_DepthAttachmentRendererID == 0)
		{
			KC_CORE_WARN("No depth attachment available for reading");
			return;
		}

		glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID);
		glReadBuffer(GL_NONE);

		glReadPixels(x, y, width, height, GL_DEPTH_COMPONENT, GL_FLOAT, outData);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void FrameBuffer::ReadStencilPixels(int x, int y, int width, int height, uint32_t* outData) const
	{
		if (m_StencilAttachmentRendererID == 0 &&
			!(m_DepthAttachmentSpecification.Format == TextureFormat::DEPTH24STENCIL8 || m_DepthAttachmentSpecification.Format == TextureFormat::DEPTH32FSTENCIL8UINT))
		{
			KC_CORE_WARN("No stencil attachment available for reading");
			return;
		}

		glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID);
		glReadBuffer(GL_NONE);

		glReadPixels(x, y, width, height, GL_STENCIL_INDEX, GL_UNSIGNED_INT, outData);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void FrameBuffer::ClearDepthAttachment()
	{
		ClearDepthAttachment(m_Specification.DepthClearValue);
	}

	void FrameBuffer::ClearDepthAttachment(float value)
	{
		if (m_DepthAttachmentRendererID == 0)
			return;

		if (Utils::IsDepthStencilFormat(m_DepthAttachmentSpecification.Format))
			glClearNamedFramebufferfi(m_RendererID, GL_DEPTH_STENCIL, 0, value, (int)m_Specification.StencilClearValue);
		else
			glClearNamedFramebufferfv(m_RendererID, GL_DEPTH, 0, &value);
	}

	void FrameBuffer::ClearColorAttachment(int attachmentIndex)
	{
		ClearColorAttachment(attachmentIndex, m_ColorAttachmentSpecifications[attachmentIndex].ClearValue);
	}

	void FrameBuffer::ClearColorAttachment(int attachmentIndex, TextureClearValue value)
	{
		if (attachmentIndex < 0 || attachmentIndex >= (int)m_ColorAttachmentsRendererID.size())
		{
			KC_CORE_WARN("Invalid color attachment index {}", attachmentIndex);
			return;
		}

		auto& spec = m_ColorAttachmentSpecifications[attachmentIndex];
		if (Utils::IsFloatFormat(spec.Format))
		{
			glClearNamedFramebufferfv(m_RendererID, GL_COLOR, attachmentIndex, &value.FloatValues[0]);
		}
		else if (Utils::IsUnsignedIntegerFormat(spec.Format))
		{
			glClearNamedFramebufferuiv(m_RendererID, GL_COLOR, attachmentIndex, &value.UIntValues[0]);
		}
		else if (Utils::IsSignedIntegerFormat(spec.Format))
		{
			glClearNamedFramebufferiv(m_RendererID, GL_COLOR, attachmentIndex, &value.IntValues[0]);
		}
		else
		{
			KC_CORE_ERROR("Unsupported format in ClearColorAttachment");
		}
	}

	void FrameBuffer::ClearStencilAttachment()
	{
		ClearStencilAttachment(m_Specification.StencilClearValue);
	}

	void FrameBuffer::ClearStencilAttachment(uint32_t value)
	{
		if (m_StencilAttachmentRendererID == 0)
			return;

		if (Utils::IsDepthStencilFormat(m_DepthAttachmentSpecification.Format))
		{
			float depth = m_Specification.DepthClearValue;
			int stencil = static_cast<int>(value);
			glClearNamedFramebufferfi(m_RendererID, GL_DEPTH_STENCIL, 0, depth, stencil);
		}
		else
		{
			glClearNamedFramebufferuiv(m_RendererID, GL_STENCIL, 0, &value);
		}
	}

	void FrameBuffer::ClearDepthStencilAttachment()
	{
		ClearDepthStencilAttachment(m_Specification.DepthClearValue, m_Specification.StencilClearValue);
	}

	void FrameBuffer::ClearDepthStencilAttachment(float depth, uint32_t stencil)
	{
		if (m_DepthAttachmentRendererID == 0 ||
			m_DepthAttachmentRendererID != m_StencilAttachmentRendererID ||
			!Utils::IsDepthStencilFormat(m_DepthAttachmentSpecification.Format))
		{
			KC_CORE_WARN("Attempted to clear combined depth-stencil, but it's not configured as such.");
			return;
		}

		glClearNamedFramebufferfi(m_RendererID, GL_DEPTH_STENCIL, 0, depth, (int)stencil);
	}

	void FrameBuffer::ClearAttachmentByName(const std::string& name)
	{
		RendererID id = GetAttachmentRendererID(name);
		if (id == 0)
		{
			KC_CORE_WARN("Cannot clear attachment '{}': not found", name);
			return;
		}
		for (size_t i = 0; i < m_ColorAttachmentsRendererID.size(); ++i)
		{
			if (m_ColorAttachmentsRendererID[i] == id)
			{
				ClearColorAttachment((int)i);
				return;
			}
		}

		if (id == m_DepthAttachmentRendererID)
		{
			ClearDepthAttachment();
			return;
		}

		if (id == m_StencilAttachmentRendererID)
		{
			ClearStencilAttachment();
			return;
		}

		KC_CORE_WARN("Attachment '{}' is not color, depth or stencil", name);
	}

	void FrameBuffer::ClearAttachments()
	{
		for (int i = 0; i < m_ColorAttachmentsRendererID.size(); i++)
			ClearColorAttachment(i);

		if (m_DepthAttachmentRendererID != 0 &&
			m_DepthAttachmentRendererID == m_StencilAttachmentRendererID &&
			Utils::IsDepthStencilFormat(m_DepthAttachmentSpecification.Format))
		{
			ClearDepthStencilAttachment();
		}
		else
		{
			ClearDepthAttachment();
			ClearStencilAttachment();
		}
	}

	RendererID FrameBuffer::GetAttachmentRendererID(const std::string& name) const
	{
		auto it = m_Attachments.find(name);
		if (it != m_Attachments.end())
			return it->second;
		else
		{
			KC_CORE_WARN("Attachment '{}' not found in frame buffer '{}'", name, m_Specification.Name);
		}

		return 0;
	}

	void FrameBuffer::Invalidate()
	{
		if (IsValid())
			DeleteTextures();

		glCreateFramebuffers(1, &m_RendererID);
		glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID);

		bool multisampled = m_Specification.Samples > 1;
		GLenum attachmentIndex = GL_COLOR_ATTACHMENT0;

		auto [width, height] = GetSize();

		for (const auto& attachment : m_Specification.Attachments.Attachments)
		{
			GLenum internalFormat = Utils::GetGLInternalFormat(attachment.Format);
			GLenum glAttachmentType = 0;

			if (attachment.Format == TextureFormat::DEPTH24STENCIL8 || attachment.Format == TextureFormat::DEPTH32FSTENCIL8UINT)
				glAttachmentType = GL_DEPTH_STENCIL_ATTACHMENT;
			else if (attachment.Format == TextureFormat::DEPTH32F)
				glAttachmentType = GL_DEPTH_ATTACHMENT;
			else if (attachment.Format == TextureFormat::STENCIL8)
				glAttachmentType = GL_STENCIL_ATTACHMENT;
			else
				glAttachmentType = attachmentIndex++; // Color attachment index

			RendererID textureID = 0;
			glCreateTextures(multisampled ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D, 1, &textureID);

			if (multisampled)
			{
				glTextureStorage2DMultisample(textureID, m_Specification.Samples, internalFormat, width, height, GL_FALSE);
			}
			else
			{
				glTextureStorage2D(textureID, 1, internalFormat, width, height);
				glTextureParameteri(textureID, GL_TEXTURE_MIN_FILTER, Utils::GetGlTextureFilter(attachment.Filter));
				glTextureParameteri(textureID, GL_TEXTURE_MAG_FILTER, Utils::GetGlTextureFilter(attachment.Filter));
				glTextureParameteri(textureID, GL_TEXTURE_WRAP_R, Utils::GetGlTextureWrap(attachment.Wrap));
				glTextureParameteri(textureID, GL_TEXTURE_WRAP_S, Utils::GetGlTextureWrap(attachment.Wrap));
				glTextureParameteri(textureID, GL_TEXTURE_WRAP_T, Utils::GetGlTextureWrap(attachment.Wrap));
			}

			glNamedFramebufferTexture(m_RendererID, glAttachmentType, textureID, 0);
			m_Attachments[attachment.Name] = textureID;

			GLenum error = glGetError();
			if (error != GL_NO_ERROR)
				KC_CORE_ERROR("OpenGL error after attaching '{}': 0x{:X}", attachment.Name, error);

			if (glAttachmentType == GL_DEPTH_ATTACHMENT)
			{
				m_DepthAttachmentRendererID    = textureID;
				m_DepthAttachmentSpecification = attachment;
			}
			else if (glAttachmentType == GL_STENCIL_ATTACHMENT)
			{
				m_StencilAttachmentRendererID    = textureID;
				m_StencilAttachmentSpecification = attachment;
			}
			else if (glAttachmentType == GL_DEPTH_STENCIL_ATTACHMENT)
			{
				m_DepthAttachmentRendererID      = textureID;
				m_StencilAttachmentRendererID    = textureID;
				m_DepthAttachmentSpecification   = attachment;
				m_StencilAttachmentSpecification = attachment;

				m_Attachments["Depth"]   = textureID;
				m_Attachments["Stencil"] = textureID;
			}
			else // color
			{
				m_ColorAttachmentsRendererID   .push_back(textureID);
				m_ColorAttachmentSpecifications.push_back(attachment);
			}		
		}

		if (m_ColorAttachmentsRendererID.size() > 1)
		{
			std::vector<GLenum> drawBuffers;
			drawBuffers.reserve(m_ColorAttachmentsRendererID.size());

			for (size_t i = 0; i < m_ColorAttachmentsRendererID.size(); i++)
				drawBuffers.push_back(GL_COLOR_ATTACHMENT0 + (GLenum)i);

			glDrawBuffers((GLsizei)drawBuffers.size(), drawBuffers.data());
		}
		else if (m_ColorAttachmentsRendererID.empty())
		{
			/// Only depth-pass
			glDrawBuffer(GL_NONE);
			glReadBuffer(GL_NONE);
		}

		KC_CORE_ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE, "Framebuffer {} is incomplete!", m_Specification.Name);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void FrameBuffer::DeleteTextures()
	{
		if (!IsValid())
			return;

		glDeleteFramebuffers(1, &m_RendererID);

		if (!m_ColorAttachmentsRendererID.empty())
			glDeleteTextures((GLsizei)m_ColorAttachmentsRendererID.size(), m_ColorAttachmentsRendererID.data());

		if (m_DepthAttachmentRendererID)
			glDeleteTextures(1, &m_DepthAttachmentRendererID);

		if (m_StencilAttachmentRendererID)
			glDeleteTextures(1, &m_StencilAttachmentRendererID);

		m_RendererID = 0;

		m_ColorAttachmentsRendererID.clear();
		m_ColorAttachmentSpecifications.clear();

		m_DepthAttachmentRendererID   = 0;
		m_StencilAttachmentRendererID = 0;

		m_Attachments.clear();
	}

	void FrameBuffer::SetDebugNames()
	{
		if (!GLAD_GL_KHR_debug)
			return;

		glObjectLabel(GL_FRAMEBUFFER, m_RendererID, -1, m_Specification.Name.c_str());

		for (size_t i = 0; i < m_ColorAttachmentsRendererID.size(); ++i)
		{
			std::string label = m_Specification.Attachments.Attachments[i].Name;
			glObjectLabel(GL_TEXTURE, m_ColorAttachmentsRendererID[i], -1, label.c_str());
		}

		if (m_DepthAttachmentRendererID)
			glObjectLabel(GL_TEXTURE, m_DepthAttachmentRendererID, -1, m_DepthAttachmentSpecification.Name.c_str());

		if (m_StencilAttachmentRendererID && m_StencilAttachmentRendererID != m_DepthAttachmentRendererID)
			glObjectLabel(GL_TEXTURE, m_StencilAttachmentRendererID, -1, m_StencilAttachmentSpecification.Name.c_str());
	}

}