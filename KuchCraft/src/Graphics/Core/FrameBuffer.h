#pragma once

#include "Graphics/Core/TextureTypes.h"

namespace KuchCraft {

	enum class FrameBufferBlitMask : uint32_t
	{
		None    = 0,
		Color   = BIT(0),
		Depth   = BIT(1),
		Stencil = BIT(2),

		All = Color | Depth | Stencil
	};

	KC_ENUM_FLAG_OPERATORS(FrameBufferBlitMask);

	struct FrameBufferTextureSpecification
	{
		std::string       Name   = "FrameBufferAttachment";
		TextureFormat     Format = TextureFormat::None;
		TextureFilter     Filter = TextureFilter::Linear;
		TextureWrap	      Wrap   = TextureWrap::Clamp;
		TextureClearValue ClearValue = {};
	};

	struct FrameBufferAttachmentSpecification
	{
		std::vector<FrameBufferTextureSpecification> Attachments;

		FrameBufferAttachmentSpecification() = default;
		FrameBufferAttachmentSpecification(std::initializer_list<FrameBufferTextureSpecification> attachments)
			: Attachments(attachments) {}
	};

	struct FrameBufferSpecification
	{
		FrameBufferAttachmentSpecification Attachments;
		std::string Name = "FrameBuffer";
		int Width  = 0;
		int Height = 0;

		int   Samples   = 1;
		bool  NoResize  = false;
		float    DepthClearValue   = 0.0f;
		uint32_t StencilClearValue = 0;
	};

	class FrameBuffer
	{
	public:
		~FrameBuffer();

		static Ref<FrameBuffer> Create(const FrameBufferSpecification& spec);

		void Bind()   const;
		void Unbind() const;

		void BindColorTexture(int slot, int attachmentIndex) const;
		void BindDepthTexture(int slot) const;
		void BindTexture(int slot, const std::string& name) const;

		void Resize(int width, int height);
		bool IsValid() const { return m_RendererID != 0; }
		void BlitTo(Ref<FrameBuffer> target, FrameBufferBlitMask mask, TextureFilter filter = TextureFilter::Nearest) const;
		void BlitToDefault(FrameBufferBlitMask mask, TextureFilter filter = TextureFilter::Nearest) const;

		void SetDrawBuffer(GLenum buffer);
		void SetDrawBuffers(std::initializer_list<GLenum> buffers);

		void ReadPixels(int attachmentIndex, int x, int y, int width, int height, void* outData) const;
		void ReadDepthPixels(int x, int y, int width, int height, float* outData) const;
		void ReadStencilPixels(int x, int y, int width, int height, uint32_t* outData) const;

		void ClearDepthAttachment();
		void ClearDepthAttachment(float value);
		void ClearColorAttachment(int attachmentIndex);
		void ClearColorAttachment(int attachmentIndex, TextureClearValue value);
		void ClearStencilAttachment();
		void ClearStencilAttachment(uint32_t value);
		void ClearDepthStencilAttachment();
		void ClearDepthStencilAttachment(float depth, uint32_t stencil);
		void ClearAttachmentByName(const std::string& name);
		void ClearAttachments();

		int GetWidth()  const { return m_Specification.Width; }
		int GetHeight() const { return m_Specification.Height; }
		std::pair<int, int> GetSize() const { return { m_Specification.Width, m_Specification.Height }; }
		const FrameBufferSpecification& GetSpecification() const { return m_Specification; }

		RendererID GetRendererID() const { return m_RendererID; }
		RendererID GetDepthAttachmentRendererID() const { return m_DepthAttachmentRendererID; }
		RendererID GetColorAttachmentRendererID(int attachmentIndex) const { return m_ColorAttachmentsRendererID[attachmentIndex]; }
		RendererID GetStencilAttachmentRendererID() const { return m_StencilAttachmentRendererID; }
		RendererID GetAttachmentRendererID(const std::string& name) const;

		bool HasDepthAttachment()   const { return m_DepthAttachmentRendererID != 0; }
		bool HasStencilAttachment() const { return m_StencilAttachmentRendererID != 0; }
		bool HasStencil() const {
			return m_StencilAttachmentRendererID != 0 || (m_DepthAttachmentRendererID != 0 &&
					(m_DepthAttachmentSpecification.Format == TextureFormat::DEPTH24STENCIL8 || m_DepthAttachmentSpecification.Format == TextureFormat::DEPTH32FSTENCIL8UINT));
		}

		bool HasColorAttachment(int attachmentIndex) const {
			return attachmentIndex >= 0 && attachmentIndex < (int)m_ColorAttachmentsRendererID.size() && m_ColorAttachmentsRendererID[attachmentIndex] != 0;
		}

	private:
		void Invalidate();
		void DeleteTextures();
		void SetDebugNames();

	private:
		FrameBufferSpecification m_Specification;
		RendererID m_RendererID = 0;

		FrameBufferTextureSpecification m_DepthAttachmentSpecification;
		RendererID m_DepthAttachmentRendererID = 0;

		FrameBufferTextureSpecification m_StencilAttachmentSpecification;
		RendererID m_StencilAttachmentRendererID = 0;

		std::vector<FrameBufferTextureSpecification> m_ColorAttachmentSpecifications;
		std::vector<RendererID> m_ColorAttachmentsRendererID;

		std::map<std::string, RendererID> m_Attachments;

	private:
		FrameBuffer(const FrameBufferSpecification& spec);

		KC_DISALLOW_COPY(FrameBuffer);
		KC_DISALLOW_MOVE(FrameBuffer);
	};

}