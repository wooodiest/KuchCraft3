#include "kcpch.h"
#include "Graphics/Core/Texture.h"

#include "Graphics/Core/GraphicsUtils.h"

#include <glad/glad.h>
#include <stb_image.h>

namespace KuchCraft {

	Texture2D::Texture2D(const TextureSpecification& spec, const std::filesystem::path& path)
		: m_Specification(spec), m_Path(path)
	{
		stbi_uc* data = nullptr;
		if (!path.empty())
		{
			int width, height, channels;

			stbi_set_flip_vertically_on_load(1);
			data = stbi_load(path.string().c_str(), &width, &height, &channels, 0);

			switch (channels)
			{
				case 1: m_Specification.Format = TextureFormat::RED8UN; break;
				case 2: m_Specification.Format = TextureFormat::RG8;    break;
				case 3: m_Specification.Format = TextureFormat::RGB;    break;
				case 4: m_Specification.Format = TextureFormat::RGBA;   break;
				default:
				{
					KC_CORE_ERROR("Unsupported image format with {0} channels", channels);
					stbi_image_free(data);
					return;
				}		
			}

			m_Specification.Width  = width;
			m_Specification.Height = height;
		}

		uint32_t mipLevels = 1;
		if (m_Specification.GenerateMips)
		{
			mipLevels = m_Specification.MipLevels > 0 ? m_Specification.MipLevels :
				Utils::CalculateMipCount(m_Specification.Width, m_Specification.Height);
		}
		m_Specification.MipLevels = mipLevels;

		GLenum internalFormat = Utils::GetGLInternalFormat(m_Specification.Format);
		GLenum filter         = Utils::GetGlTextureFilter(m_Specification.Filter);
		GLenum minFilter      = (mipLevels > 1) ? GL_LINEAR_MIPMAP_LINEAR : filter;
		GLenum wrap           = Utils::GetGlTextureWrap(m_Specification.Wrap);

		glCreateTextures(GL_TEXTURE_2D, 1, &m_RendererID);
		glTextureStorage2D(m_RendererID, mipLevels, internalFormat, m_Specification.Width, m_Specification.Height);

		glTextureParameteri(m_RendererID, GL_TEXTURE_MIN_FILTER, minFilter);
		glTextureParameteri(m_RendererID, GL_TEXTURE_MAG_FILTER, filter);
		glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_S, wrap);
		glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_T, wrap);
		
		if (data)
		{
			GLenum format = Utils::GetGLFormat(m_Specification.Format);
			GLenum type   = Utils::GetGLType(m_Specification.Format);

			glTextureSubImage2D(m_RendererID, 0, 0, 0, m_Specification.Width, m_Specification.Height, format, type, data);

			if (mipLevels > 1)
				glGenerateTextureMipmap(m_RendererID);

			stbi_image_free(data);
		}
	}

	Texture2D::~Texture2D()
	{
		if (IsValid())
			glDeleteTextures(1, &m_RendererID);
	}

	Ref<Texture2D> Texture2D::Create(const TextureSpecification& spec)
	{
		return Ref<Texture2D>(new Texture2D(spec));
	}

	Ref<Texture2D> Texture2D::Create(const std::filesystem::path& path, const TextureSpecification& spec)
	{
		return Ref<Texture2D>(new Texture2D(spec, path));
	}

	void Texture2D::Bind(int slot) const
	{
		glBindTextureUnit(slot, m_RendererID);
	}

	void Texture2D::Unbind() const
	{
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	void Texture2D::SetData(const void* data, size_t size)
	{
		KC_CORE_ASSERT(size == Utils::GetMemorySize(m_Specification.Format, m_Specification.Width, m_Specification.Height),
			"Texture2D::SetData size mismatch!");

		GLenum format = Utils::GetGLFormat(m_Specification.Format);
		GLenum type   = Utils::GetGLType(m_Specification.Format);

		glTextureSubImage2D(m_RendererID, 0, 0, 0, m_Specification.Width, m_Specification.Height, format, type, data);
	}

	void Texture2D::SetDebugName(const std::string& name)
	{
		if (!GLAD_GL_KHR_debug)
			return;

		glObjectLabel(GL_TEXTURE, m_RendererID, -1, name.c_str());
	}
}