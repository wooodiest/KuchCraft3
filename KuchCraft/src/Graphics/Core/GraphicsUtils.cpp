#include "kcpch.h"
#include "Graphics/Core/GraphicsUtils.h"

#include <glad/glad.h>

namespace KuchCraft {
	namespace Utils {

		GLenum GetGLInternalFormat(TextureFormat format)
		{
			switch (format)
			{
				case TextureFormat::RED8I:    return GL_R8I;
				case TextureFormat::RED16I:   return GL_R16I;
				case TextureFormat::RED32I:   return GL_R32I;

				case TextureFormat::RED8UN:   return GL_R8;
				case TextureFormat::RED8UI:   return GL_R8UI;
				case TextureFormat::RED16UI:  return GL_R16UI;
				case TextureFormat::RED32UI:  return GL_R32UI;
				case TextureFormat::RED32F:   return GL_R32F;

				case TextureFormat::RG8:      return GL_RG8;
				case TextureFormat::RG16F:    return GL_RG16F;
				case TextureFormat::RG32F:    return GL_RG32F;

				case TextureFormat::RGB:      return GL_RGB8;
				case TextureFormat::RGBA:     return GL_RGBA8;
				case TextureFormat::RGBA16F:  return GL_RGBA16F;
				case TextureFormat::RGBA32F:  return GL_RGBA32F;

				case TextureFormat::SRGB:     return GL_SRGB8;
				case TextureFormat::SRGBA:    return GL_SRGB8_ALPHA8;

				case TextureFormat::STENCIL8:			  return GL_STENCIL_INDEX8;
				case TextureFormat::DEPTH32F:             return GL_DEPTH_COMPONENT32F;
				case TextureFormat::DEPTH24STENCIL8:      return GL_DEPTH24_STENCIL8;
				case TextureFormat::DEPTH32FSTENCIL8UINT: return GL_DEPTH32F_STENCIL8;
			}

			KC_CORE_ASSERT(false, "Unknown TextureFormat!");
			return 0;
		}

		GLenum GetGLFormat(TextureFormat format)
		{
			switch (format)
			{
				case TextureFormat::RED8I: 
				case TextureFormat::RED16I:
				case TextureFormat::RED32I:
				case TextureFormat::RED8UN:
				case TextureFormat::RED8UI:
				case TextureFormat::RED16UI:
				case TextureFormat::RED32UI:
				case TextureFormat::RED32F:
					return GL_RED;

				case TextureFormat::RG8:
				case TextureFormat::RG16F:
				case TextureFormat::RG32F:
					return GL_RG;

				case TextureFormat::RGB:
				case TextureFormat::SRGB:
					return GL_RGB;

				case TextureFormat::RGBA:
				case TextureFormat::RGBA16F:
				case TextureFormat::RGBA32F:
				case TextureFormat::SRGBA:
					return GL_RGBA;

				case TextureFormat::DEPTH32F:
					return GL_DEPTH_COMPONENT;

				case TextureFormat::DEPTH24STENCIL8:
				case TextureFormat::DEPTH32FSTENCIL8UINT:
					return GL_DEPTH_STENCIL;

				case TextureFormat::STENCIL8: return GL_STENCIL_INDEX;
			}

			KC_CORE_ASSERT(false, "Unknown TextureFormat!");
			return 0;
		}

		GLenum GetGLType(TextureFormat format)
		{
			switch (format)
			{
				// --- SIGNED INTEGER ---
				case TextureFormat::RED8I:   return GL_BYTE;
				case TextureFormat::RED16I:  return GL_SHORT;
				case TextureFormat::RED32I:  return GL_INT;

					// --- UNSIGNED INTEGER ---
				case TextureFormat::RED8UI:  return GL_UNSIGNED_BYTE;
				case TextureFormat::RED16UI: return GL_UNSIGNED_SHORT;
				case TextureFormat::RED32UI: return GL_UNSIGNED_INT;

					// --- FLOATS & UNORMS ---
				case TextureFormat::RED8UN:
				case TextureFormat::RGBA:
				case TextureFormat::RGB:
				case TextureFormat::RG8:
				case TextureFormat::SRGB:
				case TextureFormat::SRGBA:
					return GL_UNSIGNED_BYTE;

				case TextureFormat::RED32F:
				case TextureFormat::RG16F:
				case TextureFormat::RG32F:
				case TextureFormat::RGBA16F:
				case TextureFormat::RGBA32F:
				case TextureFormat::DEPTH32F:
					return GL_FLOAT;

					// --- DEPTH/STENCIL ---
				case TextureFormat::DEPTH24STENCIL8:
					return GL_UNSIGNED_INT_24_8;
				case TextureFormat::DEPTH32FSTENCIL8UINT:
					return GL_FLOAT_32_UNSIGNED_INT_24_8_REV;
				case TextureFormat::STENCIL8: return GL_UNSIGNED_BYTE;
			}

			KC_CORE_ASSERT(false, "Unknown TextureFormat!");
			return 0;
		}

		GLenum GetGlTextureWrap(TextureWrap wrap)
		{
			switch (wrap)
			{
				case TextureWrap::Clamp:  return GL_CLAMP_TO_EDGE;
				case TextureWrap::Repeat: return GL_REPEAT;
			}

			KC_CORE_ASSERT(false, "Unknown TextureWrap!");
			return 0;
		}

		GLenum GetGlTextureFilter(TextureFilter filter)
		{
			switch (filter)
			{
				case TextureFilter::Linear: return GL_LINEAR;
				case TextureFilter::Nearest: return GL_NEAREST;
			}

			KC_CORE_ASSERT(false, "Unknown TextureFilter!");
			return 0;
		}

	}
}