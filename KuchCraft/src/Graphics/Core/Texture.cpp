#include "kcpch.h"
#include "Graphics/Core/Texture.h"

#include "Graphics/Core/GraphicsUtils.h"

#include <glad/glad.h>
#include <stb_image.h>

namespace KuchCraft {

	Texture2D::Texture2D(const TextureSpecification& spec, const std::filesystem::path& path)
		: m_Specification(spec)
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
		m_DebugName = name;

		if (!GLAD_GL_KHR_debug)
			return;

		glObjectLabel(GL_TEXTURE, m_RendererID, -1, name.c_str());
	}

	Texture2DArray::Texture2DArray(int LayerCount, const TextureSpecification& spec, const std::vector<std::filesystem::path>& paths)
		: m_LayerCount(LayerCount), m_Specification(spec)
	{
		bool readSpecAndDataFromFile = !paths.empty();

		int firstWidth, firstHeight, firstChannels;
		if (readSpecAndDataFromFile)
		{
			stbi_set_flip_vertically_on_load(1);
			stbi_uc* data = stbi_load(paths[0].string().c_str(), &firstWidth, &firstHeight, &firstChannels, 0);

			switch (firstChannels)
			{
				case 1: m_Specification.Format = TextureFormat::RED8UN; break;
				case 2: m_Specification.Format = TextureFormat::RG8;    break;
				case 3: m_Specification.Format = TextureFormat::RGB;    break;
				case 4: m_Specification.Format = TextureFormat::RGBA;   break;
				default:
				{
					KC_CORE_ERROR("Unsupported image format with {0} channels", firstChannels);
					stbi_image_free(data);
					return;
				}
			}

			m_Specification.Width  = firstWidth;
			m_Specification.Height = firstHeight;
			m_LayerCount = paths.size();

			stbi_image_free(data);
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

		glCreateTextures(GL_TEXTURE_2D_ARRAY, 1, &m_RendererID);
		glTextureStorage3D(m_RendererID, mipLevels, internalFormat, m_Specification.Width, m_Specification.Height, m_LayerCount);
		glTextureParameteri(m_RendererID, GL_TEXTURE_MIN_FILTER, minFilter);
		glTextureParameteri(m_RendererID, GL_TEXTURE_MAG_FILTER, filter);
		glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_S, wrap);
		glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_T, wrap);

		if (readSpecAndDataFromFile)
		{
			GLenum format = Utils::GetGLFormat(m_Specification.Format);
			GLenum type   = Utils::GetGLType(m_Specification.Format);

			for (int layer = 0; layer < paths.size(); ++layer)
			{
				const auto& path = paths[layer];

				int width = 0, height = 0, channels = 0;
				stbi_set_flip_vertically_on_load(1);
				stbi_uc* data = stbi_load(path.string().c_str(), &width, &height, &channels, 0);

				if (!data || width != firstWidth || height != firstHeight || channels != firstChannels)
				{
					KC_CORE_ERROR("Texture array layer {0} has incompatible dimensions or channels", layer);
					stbi_image_free(data);
					continue;
				}

				size_t layerSize = Utils::GetMemorySize(m_Specification.Format, width, height);
				SetLayerData(data, layerSize, layer);
				stbi_image_free(data);
			}

			if (mipLevels > 1)
				glGenerateTextureMipmap(m_RendererID);
		}
	}

	Texture2DArray::~Texture2DArray()
	{
		if (IsValid())
			glDeleteTextures(1, &m_RendererID);
	}

	Ref<Texture2DArray> Texture2DArray::Create(const TextureSpecification& spec, int LayerCount)
	{
		return Ref<Texture2DArray>(new Texture2DArray(LayerCount, spec, {}));
	}

	Ref<Texture2DArray> Texture2DArray::Create(const std::vector<std::filesystem::path>& paths, const TextureSpecification& spec)
	{
		int layerCount = static_cast<int>(paths.size());
		return Ref<Texture2DArray>(new Texture2DArray(layerCount, spec, paths));
	}

	void Texture2DArray::Bind(int slot) const
	{
		glBindTextureUnit(slot, m_RendererID);
	}

	void Texture2DArray::SetLayerData(const void* data, size_t size, int layer)
	{
		KC_CORE_ASSERT(layer >= 0 && layer < m_LayerCount, "Invalid layer index!");
		KC_CORE_ASSERT(size == Utils::GetMemorySize(m_Specification.Format, m_Specification.Width, m_Specification.Height),
			"Texture2DArray layerData size mismatch!");

		GLenum format = Utils::GetGLFormat(m_Specification.Format);
		GLenum type = Utils::GetGLType(m_Specification.Format);

		glTextureSubImage3D(
			m_RendererID,
			0,
			0, 0, layer,
			m_Specification.Width,
			m_Specification.Height,
			1,
			format,
			type,
			data
		);
	}

	void Texture2DArray::SetDebugName(const std::string& name)
	{
		m_DebugName = name;

		if (!GLAD_GL_KHR_debug)
			return;

		glObjectLabel(GL_TEXTURE, m_RendererID, -1, name.c_str());
	}

	TextureCube::TextureCube(const TextureSpecification& spec, const std::array<std::filesystem::path, 6>& facePaths)
		: m_Specification(spec)
	{
		bool readSpecAndDataFromFile = !facePaths[0].empty();

		int firstWidth, firstHeight, firstChannels;
		if (readSpecAndDataFromFile)
		{
			stbi_set_flip_vertically_on_load(1);
			stbi_uc* data = stbi_load(facePaths[0].string().c_str(), &firstWidth, &firstHeight, &firstChannels, 0);

			switch (firstChannels)
			{
				case 1: m_Specification.Format = TextureFormat::RED8UN; break;
				case 2: m_Specification.Format = TextureFormat::RG8;    break;
				case 3: m_Specification.Format = TextureFormat::RGB;    break;
				case 4: m_Specification.Format = TextureFormat::RGBA;   break;
				default:
				{
					KC_CORE_ERROR("Unsupported image format with {0} channels", firstChannels);
					stbi_image_free(data);
					return;
				}
			}

			m_Specification.Width  = firstWidth;
			m_Specification.Height = firstHeight;

			stbi_image_free(data);
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

		glCreateTextures(GL_TEXTURE_CUBE_MAP, 1, &m_RendererID);
		glTextureStorage2D(m_RendererID, mipLevels, internalFormat, m_Specification.Height, m_Specification.Height);
		glTextureParameteri(m_RendererID, GL_TEXTURE_MIN_FILTER, minFilter);
		glTextureParameteri(m_RendererID, GL_TEXTURE_MAG_FILTER, filter);
		glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_S, wrap);
		glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_T, wrap);

		if (readSpecAndDataFromFile)
		{
			GLenum format = Utils::GetGLFormat(m_Specification.Format);
			GLenum type   = Utils::GetGLType(m_Specification.Format);

			for (int i = 0; i < 6; i++)
			{
				int width = 0, height = 0, channels = 0;
				stbi_set_flip_vertically_on_load(1);
				stbi_uc* data = stbi_load(facePaths[i].string().c_str(), &width, &height, &channels, 0);

				if (!data || width != firstWidth || height != firstHeight || channels != firstChannels)
				{
					KC_CORE_ERROR("TextureCube face {} is invalid or incompatible", i);
					stbi_image_free(data);
					continue;
				}

				size_t dataSize = Utils::GetMemorySize(m_Specification.Format, width, height);
				SetFaceData(data, dataSize, static_cast<CubeTextureFaces>(i));
				stbi_image_free(data);
			}

			if (mipLevels > 1)
				glGenerateTextureMipmap(m_RendererID);
		}
	}

	TextureCube::~TextureCube()
	{
		if (IsValid())
			glDeleteTextures(1, &m_RendererID);
	}

	Ref<TextureCube> TextureCube::Create(const TextureSpecification& spec)
	{
		return Ref<TextureCube>(new TextureCube(spec));
	}

	Ref<TextureCube> TextureCube::Create(const std::array<std::filesystem::path, 6>& facePaths, const TextureSpecification& spec)
	{
		return Ref<TextureCube>(new TextureCube(spec, facePaths));
	}

	void TextureCube::Bind(int slot) const
	{
		glBindTextureUnit(slot, m_RendererID);
	}

	void TextureCube::SetFaceData(const void* data, size_t size, CubeTextureFaces face)
	{
		int faceIndex = static_cast<int>(face);
		KC_CORE_ASSERT(faceIndex >= 0 && faceIndex < 6, "Invalid cube map face index!");
		KC_CORE_ASSERT(size == Utils::GetMemorySize(m_Specification.Format, m_Specification.Width, m_Specification.Height),
			"TextureCube::SetFaceData size mismatch!");

		GLenum format = Utils::GetGLFormat(m_Specification.Format);
		GLenum type   = Utils::GetGLType(m_Specification.Format);

		glTextureSubImage3D(
			m_RendererID,
			0, 0, 0, faceIndex,
			m_Specification.Width,
			m_Specification.Height,
			1,
			format, type,
			data
		);
	}

	void TextureCube::SetDebugName(const std::string& name)
	{
		m_DebugName = name;

		if (!GLAD_GL_KHR_debug)
			return;

		glObjectLabel(GL_TEXTURE, m_RendererID, -1, name.c_str());
	}

}